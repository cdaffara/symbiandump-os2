// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <bautils.h>
#include <sqldb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "sqlite3.h"
#include "SqliteSymbian.h"

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlfserr test"));
_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName, "c:\\test\\t_fserr.db");
_LIT(KPrivateTestDbName, "c:\\private\\212A2C27\\t_fserr2.db");
_LIT(KSecureTestDbName, "c:[212A2C27]t_fserr3.db");

TFileName TheRmvMediaDbFileName;//The name of the file used for tests on a removable media
RFs TheFs;
RSqlDatabase TheDb;

//The next constants are used in the "blob write" test
const TInt KWriteCnt = 9;
const TInt KBlobSize = 397 * KWriteCnt;
_LIT(KAttachDb, "AttachDb");

//In order to be able to compile the test, the following variables are defined (used inside the OS porting layer, when _SQLPROFILER macro is defined)
#ifdef _SQLPROFILER
TInt TheSqlSrvProfilerFileRead = 0;
TInt TheSqlSrvProfilerFileWrite = 0;
TInt TheSqlSrvProfilerFileSync = 0;
TInt TheSqlSrvProfilerFileSetSize = 0;
#endif

///////////////////////////////////////////////////////////////////////////////////////

TBool FileExists(const TDesC& aFileName)
	{
	TEntry entry;
	return TheFs.Entry(aFileName, entry) == KErrNone;
	}

void DestroyTestEnv()
	{
	TheDb.Close();
    (void)RSqlDatabase::Delete(KSecureTestDbName);
    (void)RSqlDatabase::Delete(KPrivateTestDbName);
	(void)RSqlDatabase::Delete(KTestDbName);
	(void)RSqlDatabase::Delete(TheRmvMediaDbFileName);
	TheFs.Close();
	sqlite3SymbianLibFinalize();
	CloseSTDLIB();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DestroyTestEnv();
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

void SqliteCheck(sqlite3* aDbHandle, TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** SQLITE: Expected error: %d, got: %d\r\n"), aExpected, aValue);
		if(aDbHandle)
			{
			const char* errMsg = sqlite3_errmsg(aDbHandle);
			TPtrC8 ptr8((const TUint8*)errMsg, strlen(errMsg));
			TBuf<200> buf;
			buf.Copy(ptr8);
			RDebug::Print(_L("*** SQLITE error message: \"%S\"\r\n"), &buf);
			}
		DestroyTestEnv();
		TheTest(EFalse, aLine);
		}
	}
#define SQLITE_TEST(aDbHandle, aValue, aExpected) ::SqliteCheck(aDbHandle, aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void SetupTestEnv()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

	err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	sqlite3SymbianLibInit();
	}

TBool CheckRecord(const TDesC& aDbName, TInt aId, const TDesC& aExpectedName, TBool aOpenDb = ETrue)
	{
	if(aOpenDb)
		{
		TEST2(TheDb.Open(aDbName), KErrNone);
		}
	TBuf<64> sql;
	sql.Copy(_L("SELECT Name FROM A WHERE Id="));
	sql.AppendNum(aId);
	TSqlScalarFullSelectQuery q(TheDb);
	TBuf<20> name;
	TRAPD(err, (void)q.SelectTextL(sql, name));
	TEST2(err, KErrNone);
	if(aOpenDb)
		{
		TheDb.Close();
		}
	return name == aExpectedName;
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3419
@SYMTestCaseDesc		Test for DEF103859 "SQLITE panic, _DEBUG mode, persistent file I/O error simulation".
						The test creates a test database with one table, inserts one record.
						Then the test attempts to update the existing record while simulating file I/O failures.
						After each test iteration the database content is tested and is expected to be the same
						as it was before the test. RSqlDatabase::Exec() is used for the update operation.
@SYMTestPriority		High
@SYMTestActions			Test for DEF103859 "SQLITE panic, _DEBUG mode, persistent file I/O error simulation".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF103859
*/
void AlterDatabaseTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER,Name TEXT)"));
	TEST(err >= 0);
	TheDb.Close();
	err = KErrNotFound;
	for(TInt cnt=0;err<KErrNone;++cnt)
		{
		TheTest.Printf(_L("%d \r"), cnt);		
		for (TInt fsError=KErrNotFound;fsError>=KErrDied;--fsError)
			{
			//Preprocessing
			TEST2(TheDb.Open(KTestDbName), KErrNone);
			(void)TheDb.Exec(_L("DELETE FROM A WHERE Id=1"));
			err = TheDb.Exec(_L("INSERT INTO A(Id,Name) VALUES(1,'Name')"));
			TEST2(err, 1);
			//The test
			(void)TheFs.SetErrorCondition(fsError, cnt);
			err = TheDb.Exec(_L("UPDATE A SET Name='Name2' WHERE Id=1"));
			(void)TheFs.SetErrorCondition(KErrNone);
			if(err < 1)
				{
				TheDb.Close();//close the database to recover from the last error
				//check the database content - all bets are off in a case of an I/O error. 
				//The existing record might have been updated.
				TEST(CheckRecord(KTestDbName, 1, _L("Name")) || CheckRecord(KTestDbName, 1, _L("Name2")));
				}
			else
				{
				TEST2(err, 1);
				//check the database content has been modified by the operation. 
				TEST(CheckRecord(KTestDbName, 1, _L("Name2"), EFalse));
				TheDb.Close();
				}
			}
		}
	(void)TheFs.SetErrorCondition(KErrNone);
	TEST2(err, 1);
	//check the database content (transaction durability). 
	TEST(CheckRecord(KTestDbName, 1, _L("Name2")));
	err = RSqlDatabase::Delete(KTestDbName);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("\r\n"));
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3420
@SYMTestCaseDesc		Test for DEF103859 "SQLITE panic, _DEBUG mode, persistent file I/O error simulation".
						The test creates a test database with one table, inserts one record.
						Then the test attempts to update the existing record while simulating file I/O failures.
						After each test iteration the database content is tested and is expected to be the same
						as it was before the test. RSqlStatement::Exec() is used for the update operation.
@SYMTestPriority		High
@SYMTestActions			Test for DEF103859 "SQLITE panic, _DEBUG mode, persistent file I/O error simulation".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF103859
*/
void AlterDatabaseTest2()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER,Name TEXT)"));
	TEST(err >= 0);
	TheDb.Close();
	err = KErrNotFound;
	for(TInt cnt=0;err<KErrNone;++cnt)
		{
		TheTest.Printf(_L("%d \r"), cnt);		
		for (TInt fsError=KErrNotFound;fsError>=KErrDied;--fsError)
			{
			//Preprocessing
			TEST2(TheDb.Open(KTestDbName), KErrNone);
			(void)TheDb.Exec(_L("DELETE FROM A WHERE Id=1"));
			err = TheDb.Exec(_L("INSERT INTO A(Id,Name) VALUES(1,'Name')"));
			TEST2(err, 1);
			//The test
			(void)TheFs.SetErrorCondition(fsError, cnt);
			RSqlStatement stmt;
			err = stmt.Prepare(TheDb, _L("UPDATE A SET Name='Name2' WHERE Id=1"));
			if(err == KErrNone)
				{
				err = stmt.Exec();	
				}
			(void)TheFs.SetErrorCondition(KErrNone);
			stmt.Close();
			if(err < 1)
				{
				TheDb.Close();//close the database to recover from the last error
				//check the database content - all bets are off in a case of an I/O error. 
				//The existing record might have been updated.
				TEST(CheckRecord(KTestDbName, 1, _L("Name")) || CheckRecord(KTestDbName, 1, _L("Name2")));
				}
			else
				{
				TEST2(err, 1);
				//check the database content has been modified by the operation. 
				TEST(CheckRecord(KTestDbName, 1, _L("Name2"), EFalse));
				TheDb.Close();
				}
			}
		}
	(void)TheFs.SetErrorCondition(KErrNone);
	TEST2(err, 1);
	//check the database content has been modified by the operation. 
	TEST(CheckRecord(KTestDbName, 1, _L("Name2")));
	err = RSqlDatabase::Delete(KTestDbName);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("\r\n"));
	}

void CreateTestSecurityPolicy(RSqlSecurityPolicy& aSecurityPolicy)
    {
    TSecurityPolicy alwaysPassPolicy(TSecurityPolicy::EAlwaysPass);
    TInt err = aSecurityPolicy.Create(alwaysPassPolicy);
    TEST2(err, KErrNone);

    err = aSecurityPolicy.SetDbPolicy(RSqlSecurityPolicy::ESchemaPolicy, alwaysPassPolicy);
    TEST2(err, KErrNone);
    err = aSecurityPolicy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy, alwaysPassPolicy);
    TEST2(err, KErrNone);
    err = aSecurityPolicy.SetDbPolicy(RSqlSecurityPolicy::EReadPolicy, alwaysPassPolicy);
    TEST2(err, KErrNone);
    }

//Creates public shared, private secure and public secure databases.
void DoCreateTestDatabases(const TPtrC aDbName[], TInt aCount)
    {
    TEST(aCount > 0);
    for(TInt i=0;i<aCount;++i)
        {
        TheTest.Printf(_L("Database: \"%S\"\r\n"), &aDbName[i]);       
        (void)RSqlDatabase::Delete(aDbName[i]);
        TInt err = KErrGeneral;
        if(i == (aCount - 1))
            {
            RSqlSecurityPolicy policy;
            CreateTestSecurityPolicy(policy);
            err = TheDb.Create(aDbName[i], policy);
            policy.Close();
            }
        else
            {
            err = TheDb.Create(aDbName[i]);
            }
        TEST2(err, KErrNone);
        err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER,Name TEXT)"));
        TEST(err >= 0);
        err = TheDb.Exec(_L("INSERT INTO A(Id,Name) VALUES(1,'Name')"));
        TEST2(err, 1);
        TheDb.Close();
        }
    }

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3421
@SYMTestCaseDesc		Test for DEF103859 "SQLITE panic, _DEBUG mode, persistent file I/O error simulation".
						The test creates a test database with one table, inserts one record.
						Then the test attempts to open the database while simulating file I/O failures.
						At the end of the test the database content is tested and is expected to be the same
						as it was before the test. RSqlStatement::Open() is used in the test.
@SYMTestPriority		High
@SYMTestActions			Test for DEF103859 "SQLITE panic, _DEBUG mode, persistent file I/O error simulation".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF103859
*/
void OpenDatabaseTest()
	{
    TPtrC dbName[] = {KTestDbName(), KPrivateTestDbName(), KSecureTestDbName()};
    const TInt KDbNameCnt = sizeof(dbName) / sizeof(dbName[0]);
    DoCreateTestDatabases(dbName, KDbNameCnt);
    for(TInt k=0;k<KDbNameCnt;++k)
        {	
        TheTest.Printf(_L("Database: \"%S\"\r\n"), &dbName[k]);       
        TInt err = KErrNotFound;
        for(TInt cnt=0;err<KErrNone;++cnt)
            {		
            TheTest.Printf(_L("%d \r"), cnt);		
            for (TInt fsError=KErrNotFound;fsError>=KErrDied;--fsError)
                {
                (void)TheFs.SetErrorCondition(fsError, cnt);
                err = TheDb.Open(dbName[k]);
                (void)TheFs.SetErrorCondition(KErrNone);
                if(err != KErrNone)
                    {
                    TheDb.Close();//close the database to recover from the last error
                    //check the database content is still the same as before the "open" call
                    TEST(CheckRecord(dbName[k], 1, _L("Name")));
                    }
                else
                    {
                    TEST2(err, KErrNone);
                    //check the database content is still the same as before the operation, without closing the database
                    TEST(CheckRecord(dbName[k], 1, _L("Name"), EFalse));
                    TheDb.Close();
                    }
                }
            }
        (void)TheFs.SetErrorCondition(KErrNone);
        TEST2(err, KErrNone);
        //check the database content is the same as before the operation, after reopening the database.
        TEST(CheckRecord(dbName[k], 1, _L("Name")));
        err = RSqlDatabase::Delete(dbName[k]);
        TEST2(err, KErrNone);
        TheTest.Printf(_L("\r\n"));
        }//end of: for(TInt k=0;k<KDbNameCnt;++k)
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3434
@SYMTestCaseDesc		Test for DEF104820 "SQL, RSqlDatabase::Create() does not delete the file if fails".
						Test for DEF103859 "SQLITE panic, _DEBUG mode, persistent file I/O error simulation".
						Then the test attempts to create a database while simulating file I/O failures.
						When the test succeeds, the test verifies that the database file does exist.
@SYMTestPriority		High
@SYMTestActions			Test for DEF104820 "SQL, RSqlDatabase::Create() does not delete the file if fails".
						Test for DEF103859 "SQLITE panic, _DEBUG mode, persistent file I/O error simulation".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF103859
*/
void CreateDatabaseTest()
	{
	RSqlSecurityPolicy policy;
	CreateTestSecurityPolicy(policy);
	
	TPtrC dbName[] = {KTestDbName(), KPrivateTestDbName(), KSecureTestDbName()};
	const TInt KDbNameCnt = sizeof(dbName) / sizeof(dbName[0]);
	for(TInt k=0;k<KDbNameCnt;++k)
	    {
        TheTest.Printf(_L("Database: \"%S\"\r\n"), &dbName[k]);       
        TInt err = -1;
        for(TInt cnt=0;err<KErrNone;++cnt)
            {		
            TheTest.Printf(_L("%d \r"), cnt);		
            for (TInt fsError=KErrNotFound;fsError>=KErrDied;--fsError)
                {
                //Ideally, the database should be deleted by the SQL server, if RSqlDatabase::Create() fails.
                //But SetErrorCondition() makes the error persistent, so the SQL server will fail to delete the file.
                //This is the reason, RSqlDatabase::Delete()to be used, before simulating file I/O error.
                (void)RSqlDatabase::Delete(dbName[k]);
                (void)TheFs.SetErrorCondition(fsError, cnt);
                err = (k == (KDbNameCnt - 1)) ? TheDb.Create(dbName[k], policy) : TheDb.Create(dbName[k]);
                (void)TheFs.SetErrorCondition(KErrNone);
                TheDb.Close();
                //If err != KErrNone, the database file should have been already deleted by the server and here is 
                //the place to check that. But since the file I/O failure simulation makes the file I/O error 
                //persistent, the file cannot be deleted by the server, because the "file delete" operation also fails.
                }
            }
        (void)TheFs.SetErrorCondition(KErrNone);
        TheDb.Close();
        TEST2(err, KErrNone);
        if( k != (KDbNameCnt - 1))
            {
            TEST(FileExists(dbName[k]));
            }
        err = RSqlDatabase::Delete(dbName[k]);
        TEST2(err, KErrNone);
        TheTest.Printf(_L("\r\n"));
	    }
	policy.Close();
	}

/**
@SYMTestCaseID          PDS-SQL-UT-4189
@SYMTestCaseDesc        Test for DEF145125 "SQL, low code coverage".
                        The test creates public shared, private secure and public secure test databases.
                        Then the test opens the publich shared database and attempts to attach one of the other two
                        in a file I/O error simulation loop.
@SYMTestPriority        High
@SYMTestActions         Test for DEF145125 - "SQL, low code coverage".
@SYMTestExpectedResults The test must not fail
@SYMDEF                 DEF145125 
*/
void AttachDatabaseTest()
    {
    TPtrC dbName[] = {KTestDbName(), KPrivateTestDbName(), KSecureTestDbName()};
    const TInt KDbNameCnt = sizeof(dbName) / sizeof(dbName[0]);
    DoCreateTestDatabases(dbName, KDbNameCnt);
    for(TInt k=1;k<KDbNameCnt;++k)
        {
        TheTest.Printf(_L("Database: \"%S\"\r\n"), &dbName[k]);       
        TInt err = KErrGeneral;
        for(TInt cnt=0;err<KErrNone;++cnt)
            {
            TheTest.Printf(_L("%d \r"), cnt);       
            for(TInt fsError=KErrNotFound;fsError>=KErrDied;--fsError)
                {
                err = TheDb.Open(KTestDbName);
                TEST2(err, KErrNone);
                (void)TheFs.SetErrorCondition(fsError, cnt);
                err = TheDb.Attach(dbName[k], _L("DB2"));
                (void)TheFs.SetErrorCondition(KErrNone);
                (void)TheDb.Detach(_L("DB2"));
                TheDb.Close();//close the database to recover from the last error
                }
            }
        TEST2(err, KErrNone);
        err = RSqlDatabase::Delete(dbName[k]);
        TEST2(err, KErrNone);
        TheTest.Printf(_L("\r\n"));
        }
    }

/**
@SYMTestCaseID          PDS-SQL-UT-4190
@SYMTestCaseDesc        Test for DEF145125 "SQL, low code coverage".
                        The tests attempts to delete a database in a file I/O error simulation loop.
@SYMTestPriority        High
@SYMTestActions         Test for DEF145125 - "SQL, low code coverage".
@SYMTestExpectedResults The test must not fail
@SYMDEF                 DEF145125 
*/
void DeleteDatabaseTest()
    {
    TPtrC dbName[] = {KTestDbName(), KPrivateTestDbName(), KSecureTestDbName()};
    const TInt KDbNameCnt = sizeof(dbName) / sizeof(dbName[0]);
    DoCreateTestDatabases(dbName, KDbNameCnt);
    for(TInt k=0;k<KDbNameCnt;++k)
        {   
        TheTest.Printf(_L("Database: \"%S\"\r\n"), &dbName[k]);       
        TInt err = KErrGeneral;
        for(TInt cnt=1;err<KErrNone;++cnt)
            {
            TheTest.Printf(_L("%d \r"), cnt);
            (void)TheFs.SetErrorCondition(KErrGeneral, cnt);
            err = RSqlDatabase::Delete(dbName[k]);
            (void)TheFs.SetErrorCondition(KErrNone);
            }
        TEST2(err, KErrNone);
        err = RSqlDatabase::Delete(KTestDbName);
        TEST2(err, KErrNotFound);
        }    
    }

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3462
@SYMTestCaseDesc		Test for DEF105434 "SQL, persistent file I/O simulation, COMMIT problem".
						The test creates a test database with one table, inserts one record.
						Then the test attempts to retrieve the existing record while simulating file I/O failures.
						After each iteration, the database content is tested, that it has not been modified by the operation.
@SYMTestPriority		High
@SYMTestActions			Test for DEF105434 "SQL, persistent file I/O simulation, COMMIT problem".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF105434
*/
void SelectRecordTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER, Name TEXT)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("INSERT INTO A(Id,Name) VALUES(1,'Name')"));
	TEST2(err, 1);
	TheDb.Close();
	err = -1;
	for(TInt cnt=0;err<KErrNone;++cnt)
		{		
		TheTest.Printf(_L("%d \r"), cnt);		
		err = TheDb.Open(KTestDbName);
		TEST2(err, KErrNone);
		RSqlStatement stmt;
		(void)TheFs.SetErrorCondition(KErrGeneral, cnt);
		err = stmt.Prepare(TheDb, _L("SELECT * FROM A WHERE Id=?"));
		if(err == KErrNone)
			{
			err = stmt.BindInt(0, 1);
			if(err == KErrNone)
				{
				err = stmt.Next();
				TEST(err == KSqlAtRow || err < 0);
				if(err == KSqlAtRow)
					{
					TInt id = stmt.ColumnInt(0);
					TEST2(id, 1);
					TPtrC name;
					err = stmt.ColumnText(1, name);
					TEST2(err, KErrNone);
					TEST(name == _L("Name"));
					}
				}
			}
		(void)TheFs.SetErrorCondition(KErrNone);
		stmt.Close();
		TheDb.Close();
		//check the database content is the same as before the operation
		TEST(CheckRecord(KTestDbName, 1, _L("Name")));
		}
	(void)TheFs.SetErrorCondition(KErrNone);
	TEST(err >= 0);
	TheDb.Close();
	//check the database content is the same as before the operation, after reopening the database.
	TEST(CheckRecord(KTestDbName, 1, _L("Name")));
	err = RSqlDatabase::Delete(KTestDbName);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("\r\n"));
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3463
@SYMTestCaseDesc		Test for DEF105434 "SQL, persistent file I/O simulation, COMMIT problem".
						The test creates a test database with one table, inserts one record.
						Then the test attempts to insert another while simulating file I/O failures.
						After each iteration, the database content is tested, that it has not been modified by the operation.
						If the operation succeeds, the database content is tested again to check that the inserted record is there.
@SYMTestPriority		High
@SYMTestActions			Test for DEF105434 "SQL, persistent file I/O simulation, COMMIT problem".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF105434
*/
void InsertRecordTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER,Name TEXT)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("INSERT INTO A(Id,Name) VALUES(1,'Name')"));
	TEST2(err, 1);
	TheDb.Close();
	err = -1;
	for(TInt cnt=0;err<KErrNone;++cnt)
		{		
		TheTest.Printf(_L("%d \r"), cnt);		
		err = TheDb.Open(KTestDbName);
		TEST2(err, KErrNone);
		RSqlStatement stmt;
		(void)TheFs.SetErrorCondition(KErrGeneral, cnt);
		err = stmt.Prepare(TheDb, _L("INSERT INTO A(Id,Name) VALUES(2, 'Name2')"));
		if(err == KErrNone)
			{
			err = TheDb.Exec(_L("BEGIN TRANSACTION"));
			if(err == KErrNone)
				{
				err = stmt.Exec();
				TEST(err == 1 || err < 0);
				if(err == 1)
					{
					err = TheDb.Exec(_L("COMMIT TRANSACTION"));
					}
				}
			}
		(void)TheFs.SetErrorCondition(KErrNone);
		stmt.Close();
		if(err < 1)
			{
			TheDb.Close();//close the database to recover from the last error
			//check that the database contains the "name" record that has been inserted before the file I/O failure test.
			TEST(CheckRecord(KTestDbName, 1, _L("Name")));
			}
		else
			{
			TEST2(err, 1);
			//check the database content has been modified by the operation, without closing the database.
			TEST(CheckRecord(KTestDbName, 1, _L("Name"), EFalse));
			TEST(CheckRecord(KTestDbName, 2, _L("Name2"), EFalse));
			TheDb.Close();
			}
		}
	(void)TheFs.SetErrorCondition(KErrNone);
	TEST2(err, 1);
	//check the database content (transaction durability).
	TEST(CheckRecord(KTestDbName, 1, _L("Name")));
	TEST(CheckRecord(KTestDbName, 2, _L("Name2")));
	(void)RSqlDatabase::Delete(KTestDbName);
	TheTest.Printf(_L("\r\n"));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////         Removable media robustness test      /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_LIT8(KNameColData, "A123456789012345678901234567890");
_LIT8(KUpdatedNameColData, "1234");

//TRemovableMediaTest class is used for testing the SQLITE behaviour when the database file is created on a removable media where
//the cluster size is bigger than the page size and in case of a power failure is not guaranteed that the content
//of the last updated cluster will be preserved.
class TRemovableMediaTest
	{
	enum {KTestRecCnt = 200};
	enum {KMinCachePageSize = 512};
	
public:	
	void Run();
	
private:	
	TInt GetRemovableMediaDriveNo();
	TInt ClusterSize(TInt aDriveNo);
	void CreateDatabase(TInt aDriveNo, TInt aCachePageSize);
	void CheckRecord(sqlite3_stmt* aStmt, TInt aRecId);
	void VerifyDatabase();
	void DoTest();

	};

//Returns the number of the removable media drive, KErrNotFound otherwise.
TInt TRemovableMediaTest::GetRemovableMediaDriveNo()
	{
	for(TInt driveNo=EDriveA;driveNo<=EDriveZ;++driveNo)
		{
		TDriveInfo driveInfo;
		TInt err = TheFs.Drive(driveInfo, driveNo);
		if(err == KErrNone)
			{
			_LIT(KType1, "Not present");
			_LIT(KType2, "Unknown");
			_LIT(KType3, "Floppy");
			_LIT(KType4, "Hard disk");
			_LIT(KType5, "CD ROM");
			_LIT(KType6, "RAM disk");
			_LIT(KType7, "Flash");
			_LIT(KType8, "ROM drive");
			_LIT(KType9, "Remote drive");
			_LIT(KType10,"NAND flash");
			_LIT(KType11,"Rotating media");
			_LIT(KYes, "Yes");
			_LIT(KNo,  "No ");
			TPtrC KMediaTypeNames[] = {KType1(), KType2(), KType3(), KType4(), KType5(), KType6(), KType7(), KType8(), KType9(), KType10(), KType11()};
			TheTest.Printf(_L("Drive: %C:, %S, Removable: %S\r\n"), 'A' + driveNo, &KMediaTypeNames[driveInfo.iType], 
																	driveInfo.iDriveAtt & KDriveAttRemovable ? &KYes : &KNo);
			if(driveInfo.iDriveAtt & KDriveAttRemovable)
				{
				TheTest.Printf(_L("Removable drive to test: %C:\r\n"), 'A' + driveNo);
				return driveNo;	
				}
			}
		}
	return KErrNotFound;
	}

//Get the cluster size of aDriveNo drive	
TInt TRemovableMediaTest::ClusterSize(TInt aDriveNo)
	{
	__ASSERT_DEBUG((TUint)aDriveNo <= EDriveZ, User::Invariant());
	TVolumeIOParamInfo volIoParams;
	TInt err = TheFs.VolumeIOParam(aDriveNo, volIoParams);
	return (err == KErrNone) ? volIoParams.iClusterSize : err;
	}

//Create a test database on aDriveNo with aCachePageSize page size.
//Insert KTestRecCnt records.
void TRemovableMediaTest::CreateDatabase(TInt aDriveNo, TInt aCachePageSize)
	{
	__ASSERT_DEBUG((TUint)aDriveNo <= EDriveZ, User::Invariant());
	__ASSERT_DEBUG(aCachePageSize > 0, User::Invariant());
	TDriveUnit drvUnit(aDriveNo);
	_LIT(KDbName, "\\flashmedia.db");
	TParse parse;
	parse.Set(drvUnit.Name(), &KDbName, 0);
	TheRmvMediaDbFileName.Copy(parse.FullName());
	TBuf8<KMaxFileName + 1> dbFileName8;
	dbFileName8.Copy(TheRmvMediaDbFileName);
	(void)TheFs.Delete(TheRmvMediaDbFileName);
	
	sqlite3* dbHandle = NULL;
	TInt rc = sqlite3_open((const char*)dbFileName8.PtrZ(), &dbHandle);
	SQLITE_TEST(dbHandle, rc, SQLITE_OK);
	__ASSERT_DEBUG(dbHandle != NULL, User::Invariant());
	
	TBuf8<40> config;
	config.Copy(_L8("PRAGMA PAGE_SIZE="));
	config.AppendNum(aCachePageSize);
	rc = sqlite3_exec(dbHandle, (const char*)config.PtrZ(), 0, 0, 0);
	SQLITE_TEST(dbHandle, rc, SQLITE_OK);

	rc = sqlite3_exec(dbHandle, "CREATE TABLE A(Id INTEGER,Name TEXT)", 0, 0, 0);
	SQLITE_TEST(dbHandle, rc, SQLITE_OK);
	rc = sqlite3_exec(dbHandle, "BEGIN", 0, 0, 0);
	SQLITE_TEST(dbHandle, rc, SQLITE_OK);
	for(TInt recid=1;recid<=KTestRecCnt;++recid)
		{
		
		TBuf8<100> sql;
		sql.Copy(_L8("INSERT INTO A VALUES("));
		sql.AppendNum(recid);
		sql.Append(_L8(",'"));
		sql.Append(KNameColData);
		sql.Append(_L8("')"));
		rc = sqlite3_exec(dbHandle, (const char*)sql.PtrZ(), 0, 0, 0);
		SQLITE_TEST(dbHandle, rc, SQLITE_OK);
		}
	rc = sqlite3_exec(dbHandle, "COMMIT", 0, 0, 0);
	SQLITE_TEST(dbHandle, rc, SQLITE_OK);
	sqlite3_close(dbHandle);
	}

//Checks the content of a single record
void TRemovableMediaTest::CheckRecord(sqlite3_stmt* aStmt, TInt aRecId)
	{
	__ASSERT_DEBUG(aStmt != NULL, User::Invariant());
	TInt id = sqlite3_column_int(aStmt, 0);
	TEST2(id, aRecId);
	const TUint8* text = (const TUint8*)sqlite3_column_text(aStmt, 1);
	TPtrC8 name(text, User::StringLength(text));
	TEST(KNameColData() == name || KUpdatedNameColData() == name);
	}

//Verifies that the database content is either the same as it was before the UPDATE operation or 
//it has been updated with the new data.
void TRemovableMediaTest::VerifyDatabase()
	{
	TBuf8<KMaxFileName + 1> dbFileName8;
	dbFileName8.Copy(TheRmvMediaDbFileName);
	
	sqlite3* dbHandle = NULL;
	TInt rc = sqlite3_open((const char*)dbFileName8.PtrZ(), &dbHandle);
	SQLITE_TEST(dbHandle, rc, SQLITE_OK);
	__ASSERT_DEBUG(dbHandle != NULL, User::Invariant());
	
	sqlite3_stmt* stmtHandle = NULL;
	rc = sqlite3_prepare(dbHandle, "SELECT Id,Name FROM A", -1, &stmtHandle, 0);
	SQLITE_TEST(dbHandle, rc, SQLITE_OK);
	__ASSERT_DEBUG(stmtHandle != NULL, User::Invariant());
	
	for(TInt recid=1;recid<=KTestRecCnt;++recid)
		{
		rc = sqlite3_step(stmtHandle);
		SQLITE_TEST(dbHandle, rc, SQLITE_ROW);
		CheckRecord(stmtHandle, recid);
		}
	rc = sqlite3_step(stmtHandle);
	SQLITE_TEST(dbHandle, rc, SQLITE_DONE);
	
	sqlite3_finalize(stmtHandle);
	sqlite3_close(dbHandle);
	}

//Simulates a file system error in a loop.
//Attempts to update single record in a transaction.
//If the UPDATE operation fails - verifies the database content on each iteration.
//Note: pages are stored at the moment, not clusters. The database operations are not more robust if
//      clusters are stored in a case of a removable media.
void TRemovableMediaTest::DoTest()
	{
	TheTest.Printf(_L("Update 1 record in a file I/o simulation loop\r\n"));		
	TInt rc = -1;
	TBuf8<KMaxFileName + 1> dbFileName8;
	dbFileName8.Copy(TheRmvMediaDbFileName);
	for(TInt cnt=0;rc!=SQLITE_OK;++cnt)
		{		
		TheTest.Printf(_L("%d \r"), cnt);		
		sqlite3* dbHandle = NULL;
		rc = sqlite3_open((const char*)dbFileName8.PtrZ(), &dbHandle);
		SQLITE_TEST(dbHandle, rc, SQLITE_OK);
		__ASSERT_DEBUG(dbHandle != NULL, User::Invariant());
		(void)TheFs.SetErrorCondition(KErrCorrupt, cnt);
		rc = sqlite3_exec(dbHandle, "BEGIN IMMEDIATE", 0, 0, 0);
		if(rc == SQLITE_OK)
			{
			rc = sqlite3_exec(dbHandle, "UPDATE A SET Name='1234' WHERE Id=1", 0, 0, 0);
			if(rc == SQLITE_OK)
				{
				TInt cnt = sqlite3_changes(dbHandle);
				TEST2(cnt, 1);
				rc = sqlite3_exec(dbHandle, "COMMIT", 0, 0, 0);
				}
			}
		(void)TheFs.SetErrorCondition(KErrNone);
		sqlite3_close(dbHandle);
		if(rc != SQLITE_OK)
			{
			VerifyDatabase();
			}
		}
	TEST2(rc, SQLITE_OK);
	VerifyDatabase();
	}
	
void TRemovableMediaTest::Run()
	{
	TInt driveNo = GetRemovableMediaDriveNo();
	if(driveNo == KErrNotFound)
		{
		TheTest.Printf(_L("No removable media discovered. Test case not executed.\r\n"));	
		return;
		}
	TInt clusterSize = ClusterSize(driveNo);
	if(clusterSize < 0)
		{
		TheTest.Printf(_L("Error %d retrieving the cluster size of drive %C. Test case not executed.\r\n"), clusterSize, 'A' + driveNo);
		return;
		}
	if(clusterSize <= KMinCachePageSize)
		{
		TheTest.Printf(_L("Cluster size: %d. No appropriate cache page size found. Test case not executed.\r\n"), clusterSize);	
		return;
		}
		
	TheTest.Printf(_L("Cluster size: %d. Cache page size %d.\r\nBegin test.\r\n"), clusterSize, KMinCachePageSize);	
	CreateDatabase(driveNo, KMinCachePageSize);
	DoTest();
	(void)TheFs.Delete(TheRmvMediaDbFileName);
	TheTest.Printf(_L("End test.\r\n"));
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3516
@SYMTestCaseDesc		Removable media robustness test
						The test creates a test database with a table with some records. Then the test verifies
						that the database content cannot be corrupted by file I/O failures during database updates, 
						when the database file is on a removable media and the media cluster size is bigger than the 
						database page size.
@SYMTestPriority		High
@SYMTestActions			Removable media robustness test
@SYMTestExpectedResults The test must not fail
@SYMREQ					REQ7913
*/
void RemovableMediaRobustnessTest()
	{
	TRemovableMediaTest removableMediaTest;
	removableMediaTest.Run();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4044
@SYMTestCaseDesc		RSqlDatabase::Size(TSize&), file I/O error simulation test.
						The test creates a database and executes RSqldatabase::Size(TSize&)
						during a file I/O error simulation. The database should not be corrupted 
						by the call.
@SYMTestPriority		High
@SYMTestActions			RSqlDatabase::Size(TSize&), file I/O error simulation test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10407
*/
void SizeTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("BEGIN;CREATE TABLE A(Id INTEGER,Data BLOB);INSERT INTO A VALUES(1, x'11223344');COMMIT;"));
	TEST(err >= 0);
	RSqlDatabase::TSize size1 = {-1, -1};
	err = TheDb.Size(size1);
	TEST2(err, KErrNone);
	TEST(size1.iSize > 0);
	TEST2(size1.iFree, 0);
	TheDb.Close();
	//"File I/O" error simulation loop
	err = KErrCorrupt;
	for(TInt cnt=0;err<KErrNone;++cnt)
		{
		TheTest.Printf(_L("%d \r"), cnt);		
		TEST2(TheDb.Open(KTestDbName), KErrNone);
		(void)TheFs.SetErrorCondition(KErrCorrupt, cnt);
		RSqlDatabase::TSize size2 = {-1, -1};
		err = TheDb.Size(size2);
		(void)TheFs.SetErrorCondition(KErrNone);
		TheDb.Close();
		if(err == KErrNone)
			{
			TEST(size2.iSize == size1.iSize);
			TEST(size2.iFree == size1.iFree);
			break;
			}
		else
			{
			//check the database content - all bets are off in a case of an I/O error. 
			TEST2(TheDb.Open(KTestDbName), KErrNone);
			TSqlScalarFullSelectQuery q(TheDb);
			TInt recCnt = 0;
			TRAPD(err2, recCnt = q.SelectIntL(_L8("SELECT COUNT(*) FROM A")));
			TheDb.Close();
			TEST2(err2, KErrNone);
			TEST2(recCnt, 1);
			}
		}
	(void)RSqlDatabase::Delete(KTestDbName);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4045
@SYMTestCaseDesc		RSqlDatabase::Compact(), file I/O error simulation test.
						The test creates a database and executes RSqlDatabase::Compact()
						during a file I/O error simulation. The database should not be corrupted 
						by the call.
@SYMTestPriority		High
@SYMTestActions			RSqlDatabase::Compact(), file I/O error simulation test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10405
*/
void CompactTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	_LIT8(KConfig, "compaction=manual");
	TInt err = TheDb.Create(KTestDbName, &KConfig);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER,Data BLOB)"));
	TEST(err >= 0);
	//Insert records
	err = TheDb.Exec(_L8("BEGIN"));
	TEST(err >= 0);
	const TInt KRecLen = 1000;
	TBuf8<KRecLen> sqlfmt;
	sqlfmt.Copy(_L8("INSERT INTO A VALUES(%d,x'"));
	for(TInt j=0;j<(KRecLen-50);++j)
		{
		sqlfmt.Append(_L8("A"));
		}
	sqlfmt.Append(_L8("')"));
	const TInt KRecCount = 100;	
	for(TInt i=0;i<KRecCount;++i)
		{
		TBuf8<KRecLen> sql;
		sql.Format(sqlfmt, i + 1);
		err = TheDb.Exec(sql);
		TEST2(err, 1);
		}
	err = TheDb.Exec(_L8("COMMIT"));
	TEST(err >= 0);
	//Free some space
	const TInt KDeletedRecCnt = KRecCount - 10;
	err = TheDb.Exec(_L8("DELETE FROM A WHERE Id > 10"));
	TEST(err >= 0);
	//Get the database size
	RSqlDatabase::TSize size;
	err = TheDb.Size(size);
	TEST2(err, KErrNone);
	TheDb.Close();
	TEST(size.iSize > 0);
	TEST(size.iFree > 0);
	//"File I/O" error simulation loop
	err = KErrCorrupt;
	for(TInt cnt=0;err<KErrNone;++cnt)
		{
		TheTest.Printf(_L("%d \r"), cnt);		
		TEST2(TheDb.Open(KTestDbName), KErrNone);
		(void)TheFs.SetErrorCondition(KErrCorrupt, cnt);
		err = TheDb.Compact(RSqlDatabase::EMaxCompaction);
		(void)TheFs.SetErrorCondition(KErrNone);
		TheDb.Close();
		if(err == KErrNone)
			{
			break;
			}
		else
			{
			//check the database content - all bets are off in a case of an I/O error. 
			//The database maight have been compacted, so - no check for that.
			TEST2(TheDb.Open(KTestDbName), KErrNone);
			TSqlScalarFullSelectQuery q(TheDb);
			TInt recCnt = 0;
			TRAPD(err2, recCnt = q.SelectIntL(_L8("SELECT COUNT(*) FROM A")));
			TheDb.Close();
			TEST2(err2, KErrNone);
			TEST2(recCnt, (KRecCount - KDeletedRecCnt));
			}
		}
	TheTest.Printf(_L("\r\n"));
	//Check that the database has been really compacted
	TEST2(TheDb.Open(KTestDbName), KErrNone);
	RSqlDatabase::TSize size2;
	err = TheDb.Size(size2);
	TEST2(err, KErrNone);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	TEST(size.iSize > size2.iSize);
	TEST2(size2.iFree, 0);
	}

void DoBlobWriteStreamTestL(TBool aAttachDb)
	{
	RSqlBlobWriteStream strm;
	CleanupClosePushL(strm);
	if(aAttachDb)
		{
		strm.OpenL(TheDb, _L("A"), _L("Data"), 1, KAttachDb);
		}
	else
		{
		strm.OpenL(TheDb, _L("A"), _L("Data"), 1);
		}

	TBuf8<KBlobSize / KWriteCnt> data;
	data.SetLength(KBlobSize / KWriteCnt);
	data.Fill(0xA5);
	
	for(TInt i=0;i<KWriteCnt;++i)
		{
		strm.WriteL(data);
		}
	
	strm.CommitL();
	
	CleanupStack::PopAndDestroy(&strm);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4089
@SYMTestCaseDesc		RSqlBlobWriteStream::WriteL(), file I/O error simulation test.
						The test creates a database and executes RSqlBlobWriteStream::WriteL()
						during a file I/O error simulation. The database should not be corrupted 
						by the call.
@SYMTestPriority		High
@SYMTestActions			RSqlBlobWriteStream::WriteL(), file I/O error simulation test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ10418
*/
void BlobWriteStreamTest(TBool aAttachDb)
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER,Data BLOB)"));
	TEST2(err, 1);
	TBuf8<100> sql;
	sql.Format(_L8("INSERT INTO A VALUES(1, zeroblob(%d))"), KBlobSize);
	err = TheDb.Exec(sql);
	TEST2(err, 1);
	TheDb.Close();
	
	err = KErrCorrupt;
	for(TInt cnt=0;err<KErrNone;++cnt)
		{
		TheTest.Printf(_L("%d \r"), cnt);		
		TEST2(TheDb.Open(KTestDbName), KErrNone);
		if(aAttachDb)
			{
			TEST2(TheDb.Attach(KTestDbName, KAttachDb), KErrNone);	
			}
		(void)TheFs.SetErrorCondition(KErrCorrupt, cnt);
		TRAP(err, DoBlobWriteStreamTestL(aAttachDb));
		(void)TheFs.SetErrorCondition(KErrNone);
		if(aAttachDb)
			{
			TEST2(TheDb.Detach(KAttachDb), KErrNone);	
			}
		TheDb.Close();
		}
	TheTest.Printf(_L("\r\n"));

	TEST2(TheDb.Open(KTestDbName), KErrNone);
	
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TPtrC8 data;
	err = stmt.ColumnBinary(1, data);
	TEST2(err, KErrNone);
	TEST2(data.Length(), KBlobSize);
	for(TInt j=0;j<KBlobSize;++j)
		{
		TUint8 d = data[j];
		TEST2(d, 0xA5);	
		}
	stmt.Close();
	
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	}

void DoBlobReadStreamTestL(TBool aAttachDb, TDes8& aDes)
	{
	RSqlBlobReadStream strm;
	CleanupClosePushL(strm);
	if(aAttachDb)
		{
		strm.OpenL(TheDb, _L("A"), _L("Data"), 1, KAttachDb);
		}
	else
		{
		strm.OpenL(TheDb, _L("A"), _L("Data"), 1);
		}

	TBuf8<KBlobSize / KWriteCnt> data;
	aDes.SetLength(0);
	
	for(TInt i=0;i<KWriteCnt;++i)
		{
		strm.ReadL(data);
		aDes.Append(data);
		}
	
	CleanupStack::PopAndDestroy(&strm);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4090
@SYMTestCaseDesc		RSqlBlobReadStream::ReadL(), file I/O error simulation test.
						The test creates a database and executes RSqlBlobReadStream::ReadL()
						during a file I/O error simulation. The database should not be corrupted 
						by the call.
@SYMTestPriority		High
@SYMTestActions			RSqlBlobReadStream::ReadL(), file I/O error simulation test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ10410
                        REQ10411
*/
void BlobReadStreamTest(TBool aAttachDb)
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER,Data BLOB)"));
	TEST2(err, 1);
	TBuf8<100> sql;
	sql.Format(_L8("INSERT INTO A VALUES(1, zeroblob(%d))"), KBlobSize);
	err = TheDb.Exec(sql);
	TEST2(err, 1);
	TRAP(err, DoBlobWriteStreamTestL(EFalse));
	TEST2(err, KErrNone);
	TheDb.Close();
	
	HBufC8* buf = HBufC8::New(KBlobSize);
	TEST(buf != NULL);
	TPtr8 bufptr = buf->Des();
	
	err = KErrCorrupt;
	for(TInt cnt=0;err<KErrNone;++cnt)
		{
		TheTest.Printf(_L("%d \r"), cnt);		
		TEST2(TheDb.Open(KTestDbName), KErrNone);
		if(aAttachDb)
			{
			TEST2(TheDb.Attach(KTestDbName, KAttachDb), KErrNone);	
			}
		(void)TheFs.SetErrorCondition(KErrCorrupt, cnt);
		TRAP(err, DoBlobReadStreamTestL(aAttachDb, bufptr));
		(void)TheFs.SetErrorCondition(KErrNone);
		if(aAttachDb)
			{
			TEST2(TheDb.Detach(KAttachDb), KErrNone);	
			}
		TheDb.Close();
		}
	TheTest.Printf(_L("\r\n"));

	TEST2(bufptr.Length(), KBlobSize);
	for(TInt j=0;j<KBlobSize;++j)
		{
		TUint8 d = bufptr[j];
		TEST2(d, 0xA5);	
		}
		
	delete buf;
		
	(void)RSqlDatabase::Delete(KTestDbName);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DoTests()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3419 Alter database during file I/O error "));
	AlterDatabaseTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3420 Alter database during file I/O error (using statement object) "));
	AlterDatabaseTest2();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3421 Open database during file I/O error "));
	OpenDatabaseTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3434 Create database during file I/O error "));
	CreateDatabaseTest();
    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4189 Attach database during file I/O error "));
    AttachDatabaseTest();
    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4190 Delete database during file I/O error "));
    DeleteDatabaseTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3462 Select record test during file I/O error "));
	SelectRecordTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3463 Insert record test during file I/O error "));
	InsertRecordTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3516 Removable Media robustness test "));
	RemovableMediaRobustnessTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4044 Database size test during file I/O error"));	
	SizeTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4045 Compact database test during file I/O error"));	
	CompactTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4089 RSqlBlobWriteStream::WriteL() test during file I/O error"));	
	BlobWriteStreamTest(EFalse);
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4089 RSqlBlobWriteStream::WriteL()+attached database test during file I/O error"));	
	BlobWriteStreamTest(ETrue);
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4090 RSqlBlobReadStream::ReadL() test during file I/O error"));	
	BlobReadStreamTest(EFalse);
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4090 RSqlBlobReadStream::ReadL()+attached database test during file I/O error"));	
	BlobReadStreamTest(ETrue);
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	SetupTestEnv();
	DoTests();
	DestroyTestEnv();
	
	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;
	
	User::Heap().Check();
	return KErrNone;
	}
