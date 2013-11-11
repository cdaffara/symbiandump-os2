// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <sqldb.h>
#include <f32file.h>
///////////////////////////////////////////////////////////////////////////////////////

static RFs TheFs;
static RTest TheTest(_L("t_sqldefect2 test"));
static RSqlDatabase TheDb1;
static RSqlDatabase TheDb2;

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDatabase1, "c:\\test\\t_sqldefect2.db");
_LIT(KTestDatabaseJournal1, "c:\\test\\t_sqldefect2.db-journal");
_LIT(KServerTempDir, "c:\\private\\10281e17\\temp\\");

///////////////////////////////////////////////////////////////////////////////////////

//Deletes all created test files.
void DestroyTestEnv()
	{
    TheDb2.Close();
    TheDb1.Close();
	(void)RSqlDatabase::Delete(KTestDatabase1);
	TheFs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

//Creates file session instance and the test directory
void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

	err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4154
@SYMTestCaseDesc        Test for DEF143062: SQL, "CREATE INDEX" sql crashes SQL server.
                        The test creates a database with one empty table and establishes two connections
                        to that database. Then, while the first connection is at the middle of a read
                        transaction, the second connection attempts to create an index.
                        If the defect is not fixed, the SQL server will crash.
@SYMTestPriority        High
@SYMTestActions         DEF143062: SQL, "CREATE INDEX" sql crashes SQL server.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF143062
*/
void DEF143062()
    {
    (void)RSqlDatabase::Delete(KTestDatabase1);
    TInt err = TheDb1.Create(KTestDatabase1);
    TEST2(err, KErrNone);
    err = TheDb1.Exec(_L("CREATE TABLE T0(Thread INTEGER, LocalIndex INTEGER, Inserts INTEGER, Updates INTEGER, IndexMod8 INTEGER)"));
    TEST(err >= 0);
    
    err = TheDb2.Open(KTestDatabase1);
    TEST2(err, KErrNone);

    RSqlStatement stmt;
    err = stmt.Prepare(TheDb1, _L8("SELECT COUNT(Thread) FROM T0 WHERE Thread = 0"));
    TEST2(err, KErrNone);
    err = stmt.Next();
    TEST2(err, KSqlAtRow);
    
    err = TheDb2.Exec(_L8("CREATE INDEX T0INDEX ON T0(Thread,IndexMod8)"));//crashes the SQL server if the defect is not fixed 
    TEST2(err, KSqlErrLocked);
    
    stmt.Close();
    
    TheDb2.Close();
    TheDb1.Close();
    err = RSqlDatabase::Delete(KTestDatabase1);
    TEST2(err, KErrNone);
    }

/**
@SYMTestCaseID          PDS-SQL-CT-4155
@SYMTestCaseDesc        Test for DEF143061: SQL, SQLITE_DEFAULT_JOURNAL_SIZE_LIMIT value is too big.
                                         The test verifies that after comitting a big transaction, the journal file size is made equal the 
                                          max journal file size limit of 64Kb.
@SYMTestPriority        High
@SYMTestActions         DEF143061: SQL, SQLITE_DEFAULT_JOURNAL_SIZE_LIMIT value is too big..
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF143061
*/
void DEF143061()
    {
    (void)RSqlDatabase::Delete(KTestDatabase1);
    //"Auto" compaction is used in order to see how the journal file is immediatelly used.
    _LIT8(KConfig, "compaction=auto");
    TInt err = TheDb1.Create(KTestDatabase1, &KConfig);
    TEST2(err, KErrNone);
    err = TheDb1.Exec(_L("CREATE TABLE A(I INTEGER, B BLOB)"));
    TEST(err >= 0);

    const TInt KBlobSize = 100000;//bigger than the journal size limit
    HBufC8* buf = HBufC8::New(KBlobSize);
    TEST(buf != NULL);
    TPtr8 ptr = buf->Des();
    ptr.SetLength(KBlobSize);
        
    RSqlStatement stmt;
    err = stmt.Prepare(TheDb1, _L("INSERT INTO A VALUES(1, :Prm)"));
    TEST2(err, KErrNone);
    ptr.Fill(TChar('N'));
    err = stmt.BindBinary(0, ptr);
    TEST2(err, KErrNone);
    err = stmt.Exec();
    TEST2(err, 1);
    stmt.Close();
    
    //Try to update the BLOB in the record that was just inserted. This operation should create a big journal file.
    err = stmt.Prepare(TheDb1, _L("UPDATE A SET B=:Prm WHERE I=1"));
    TEST2(err, KErrNone);
    ptr.Fill(TChar('Y'));
    err = stmt.BindBinary(0, ptr);
    TEST2(err, KErrNone);
    err = stmt.Exec();
    TEST2(err, 1);
    stmt.Close();
    
    //Check the journal file size. It should be less than the 64Kb limit defined in sqlite_macro.mmh file.  
    TEntry entry;
    err = TheFs.Entry(KTestDatabaseJournal1, entry);
    TEST2(err, KErrNone);
    TInt64 fsize = entry.FileSize();
    TEST(fsize <= SQLITE_DEFAULT_JOURNAL_SIZE_LIMIT);
    
    delete buf;
    TheDb1.Close();
    err = RSqlDatabase::Delete(KTestDatabase1);
    TEST2(err, KErrNone);
    }

/**
@SYMTestCaseID          PDS-SQL-CT-4156
@SYMTestCaseDesc        Test for DEF143150: SQL, strftime() returns incorrect result.
                        The test takes the current universal time (using TTime) 
                        and the current time retrieved from the SQL  server.
                        The test compares the times and expects the difference to be no more than
                        1 second. 
@SYMTestPriority        High
@SYMTestActions         DEF143150: SQL, strftime() returns incorrect result
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF143150
*/
void DEF143150()
    {
    (void)RSqlDatabase::Delete(KTestDatabase1);
    TInt err = TheDb1.Create(KTestDatabase1);
    TEST2(err, KErrNone);

    //Home date & time
    TBuf<50> dtstr1;
    TTime time;
    time.UniversalTime();
    TDateTime dt = time.DateTime();
    
    RSqlStatement stmt;
    err = stmt.Prepare(TheDb1, _L("SELECT strftime('%Y-%m-%d,%H:%M:%S','now')"));
    TEST2(err, KErrNone);
    err = stmt.Next();
    TEST2(err, KSqlAtRow);
    
    //SQLite date & time
    TBuf<50> dtstr2;
    err = stmt.ColumnText(0, dtstr2);
    TEST2(err, KErrNone);
    stmt.Close();

    TheDb1.Close();
    err = RSqlDatabase::Delete(KTestDatabase1);
    TEST2(err, KErrNone);
    
    dtstr1.Format(_L("%04d-%02d-%02d,%02d:%02d:%02d"), dt.Year(), dt.Month() + 1, dt.Day() + 1, dt.Hour(), dt.Minute(), dt.Second());
    TheTest.Printf(_L("Universal date&time=\"%S\"\n"), &dtstr1);
    TheTest.Printf(_L("SQLite    date&time=\"%S\"\n"), &dtstr2);
    
    //Comapare and fail if dates are not equal (+- 1 second)
    TLex lex;
    lex = dtstr2.Mid(0, 4);
    TInt sqlyear;
    err = lex.Val(sqlyear);
    TEST2(err, KErrNone);
    
    lex = dtstr2.Mid(5, 2);
    TInt sqlmonth;
    err = lex.Val(sqlmonth);
    TEST2(err, KErrNone);
    
    lex = dtstr2.Mid(8, 2);
    TInt sqlday;
    err = lex.Val(sqlday);
    TEST2(err, KErrNone);
    
    lex = dtstr2.Mid(11, 2);
    TInt sqlhour;
    err = lex.Val(sqlhour);
    TEST2(err, KErrNone);
    
    lex = dtstr2.Mid(14, 2);
    TInt sqlminute;
    err = lex.Val(sqlminute);
    TEST2(err, KErrNone);
    
    lex = dtstr2.Mid(17, 2);
    TInt sqlsecond;
    err = lex.Val(sqlsecond);
    TEST2(err, KErrNone);
    
    TDateTime sqldt(sqlyear, (TMonth)(sqlmonth - 1), sqlday - 1, sqlhour, sqlminute, sqlsecond, 0);
    TTime sqltime(sqldt);
    TTimeIntervalSeconds diff;
    err = sqltime.SecondsFrom(time, diff);
    TEST2(err, KErrNone);
    TEST(diff.Int() <= 1);
    }

static TInt KillProcess(const TDesC& aProcessName)
    {
    TFullName name;
    TBuf<64> pattern(aProcessName);
    TInt length = pattern.Length();
    pattern += _L("*");
    TFindProcess procFinder(pattern);

    while (procFinder.Next(name) == KErrNone)
        {
        if (name.Length() > length)
            {//If found name is a string containing aProcessName string.
            TChar c(name[length]);
            if (c.IsAlphaDigit() ||
                c == TChar('_') ||
                c == TChar('-'))
                {
                // If the found name is other valid application name
                // starting with aProcessName string.
                continue;
                }
            }
        RProcess proc;
        if (proc.Open(name) == KErrNone)
            {
            proc.Kill(0);
            }
        proc.Close();
        }
    return KErrNone;
    }

/**
@SYMTestCaseID          PDS-SQL-CT-4210
@SYMTestCaseDesc        Test for the change "Temp files created during sql operations are not deleted after rebooting the phone" 
@SYMTestPriority        High
@SYMTestActions         Kill the sql server
                        Create two temp files in sql server's private directory
                        Start the sql server
                        Test that the temp files do not exist.
@SYMTestExpectedResults Test must not fail
*/
void DeleteTempFile()
    {   
    _LIT(KSqlSrvName, "sqlsrv.exe");
    _LIT(KTempFile1, "TMP00052.$$$");
    _LIT(KTempFile2, "TMP00044.$$$");
    
    KillProcess(KSqlSrvName);
 
    //Create two temp file in c:\\private\\10281e17\\temp\\ folder
    TInt err = TheFs.MkDir(KServerTempDir);
    TEST(err == KErrNone || err == KErrAlreadyExists);
    RFile file;
    TFileName filename1(KServerTempDir);
    TFileName filename2(KServerTempDir);
    filename1.Append(KTempFile1);
    filename2.Append(KTempFile2);
    err = file.Replace(TheFs, filename1, 0);
    file.Close();
    TEST2(err, KErrNone);
    err = file.Replace(TheFs, filename2, 0);
    file.Close();
    TEST2(err, KErrNone);
    
    //Create a database that should start sql server
    err = TheDb1.Create(KTestDatabase1);
    TEST(err == KErrNone || err == KErrAlreadyExists);
    //Test that the temp files have been deleted during server's start-up
    TUint dummy;
    err = TheFs.Att(filename1, dummy);
    TEST2(err, KErrNotFound);
    err = TheFs.Att(filename2, dummy);
    TEST2(err, KErrNotFound);
    
    TheDb1.Close();
    err = RSqlDatabase::Delete(KTestDatabase1);
    TEST2(err, KErrNone);
    }

TInt TempFilesCount()
	{
    _LIT(KServerTempDirMask, "c:\\private\\10281e17\\temp\\*.*");
	CDir* dir = NULL;
	TInt err = TheFs.GetDir(KServerTempDirMask, KEntryAttNormal, ESortNone, dir);
	TEST2(err, KErrNone);
	TInt tmpFileCount = dir->Count();
	delete dir;
	return tmpFileCount;
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4211
@SYMTestCaseDesc        Test for the change "Temp files created during sql operations are not deleted after rebooting the phone" 
@SYMTestPriority        High
@SYMTestActions         The test creates a database and runs a set of statements that
						will lead to a delayed creation of a temp file.
						At the end the test checks that the temp file was created.
@SYMTestExpectedResults Test must not fail
*/
void TempFileTest()
	{
    (void)RSqlDatabase::Delete(KTestDatabase1);
    TInt err = TheDb1.Create(KTestDatabase1);
    TEST2(err, KErrNone);
    //Get the number of the files in the SQL temp directory 
	TInt tmpFileCount = TempFilesCount();
    //    
    err = TheDb1.Exec(_L("CREATE TABLE t1(x UNIQUE); INSERT INTO t1 VALUES(1)"));
    TEST(err >= 0);
    err = TheDb1.Exec(_L("BEGIN; UPDATE t1 SET x = 2; UPDATE t1 SET x = 3; COMMIT"));
    TEST(err >= 0);
    //Check that a temp file really was created
	TInt tmpFileCount2 = TempFilesCount();
	TEST(tmpFileCount2 > tmpFileCount);
    //
    TheDb1.Close();
    err = RSqlDatabase::Delete(KTestDatabase1);
    TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4213
@SYMTestCaseDesc        Tests the ability of the SQL server to store empty strings and retrieve them as 
						text column values, not NULLs.
						Change: ou1cimx1#504388. 
@SYMTestPriority        High
@SYMTestActions         The test creates a database and a table and stores there empty strings.
						Then the test retrieves the stored column values and verifies that the column type is
						"text", not "null".
@SYMTestExpectedResults Test must not fail
*/
void EmptyTextColumnTest()
	{
	_LIT8(KEncUtf16, "encoding=\"UTF-16\"");
	_LIT8(KEncUtf8, "encoding=\"UTF-8\"");
	TPtrC8 enc[] = {KEncUtf16(), KEncUtf8()};
	for(TInt i=0;i<(sizeof(enc)/sizeof(enc[0]));++i)
		{
		(void)RSqlDatabase::Delete(KTestDatabase1);
		TInt err = TheDb1.Create(KTestDatabase1, &enc[i]);
		TEST2(err, KErrNone);
		//Insert records with empty text column values using RSqlDatabase::Exec()
	    err = TheDb1.Exec(_L("CREATE TABLE A(ID INTEGER, T TEXT)"));
	    TEST(err >= 0);
	    err = TheDb1.Exec(_L("INSERT INTO A VALUES(1, '')"));
	    TEST2(err, 1);
	    err = TheDb1.Exec(_L8("INSERT INTO A VALUES(2, '')"));
	    TEST2(err, 1);
		//Insert a record with empty text column value using RSqlParamWriteStream
	    RSqlStatement stmt;
	    err = stmt.Prepare(TheDb1, _L("INSERT INTO A(ID, T) VALUES(:P1, :P2)"));
	    TEST2(err, KErrNone);
		err = stmt.BindInt(0, 3);
	    TEST2(err, KErrNone);
	    RSqlParamWriteStream strm;
	    err = strm.BindText(stmt, 1);
	    TEST2(err, KErrNone);
	    TRAP(err, strm.WriteL(KNullDesC));
	    TEST2(err, KErrNone);
	    strm.Close();
	    err = stmt.Exec();
	    TEST2(err, 1);
	    stmt.Close();
		//Insert records with empty text column values using RSqlStatement::Bind()
	    err = stmt.Prepare(TheDb1, _L("INSERT INTO A(ID, T) VALUES(:P1, :P2)"));
	    TEST2(err, KErrNone);
		err = stmt.BindInt(0, 4);
	    TEST2(err, KErrNone);
		err = stmt.BindText(1, KNullDesC);
	    TEST2(err, KErrNone);
	    err = stmt.Exec();
	    TEST2(err, 1);
	    //
	    err = stmt.Reset();
	    TEST2(err, KErrNone);
		err = stmt.BindInt(0, 5);
	    TEST2(err, KErrNone);
	    _LIT(KEmptyStr, "");
		err = stmt.BindText(1, KEmptyStr);
	    TEST2(err, KErrNone);
	    err = stmt.Exec();
	    TEST2(err, 1);
	    stmt.Close();
	    //Read the empty text column values
	    err = stmt.Prepare(TheDb1, _L("SELECT T FROM A"));
	    TEST2(err, KErrNone);
	    TInt cnt = 0;
	    while((err = stmt.Next()) == KSqlAtRow)
	    	{
			++cnt;
			TPtrC val;
			err = stmt.ColumnText(0, val);
			TEST2(err, KErrNone);
			TEST2(val.Length(), 0);
			TSqlColumnType type = stmt.ColumnType(0);
			TEST2(type, ESqlText);
	    	}
	    stmt.Close();
	    TEST2(err, KSqlAtEnd);
	    TEST2(cnt, 5);
	    //
	    TheDb1.Close();
	    err = RSqlDatabase::Delete(KTestDatabase1);
	    TEST2(err, KErrNone);
		}
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4214
@SYMTestCaseDesc        Test for the change "After *#7370# Java apps are not preinstalled again" 
@SYMTestPriority        High
@SYMTestActions         The test makes sure there are no issues if the temp folder is removed after the server 
                        has already started. The test performs the following actions - 
                        1. Delete the 'temp' directory.
                        2. Create a transaction which creates temp files.
                        3. Check 'temp' folder exists at the end
@SYMTestExpectedResults Test must not fail
*/
void DeleteTempFolder()
    {
    //1. Delete 'temp' folder
    TInt err = TheFs.RmDir(KServerTempDir);
    TEST2(err, KErrNone);
	
    //2. Create a transaction which creates temp files.
    (void)RSqlDatabase::Delete(KTestDatabase1);
    err = TheDb1.Create(KTestDatabase1);
    TEST2(err, KErrNone);
    
    err = TheDb1.Exec(_L("CREATE TABLE t1(x UNIQUE); INSERT INTO t1 VALUES(1)"));
    TEST(err >= 0);
    err = TheDb1.Exec(_L("BEGIN; UPDATE t1 SET x = 2; UPDATE t1 SET x = 3; COMMIT"));
    TEST(err >= 0);
    
    TheDb1.Close();
    err = RSqlDatabase::Delete(KTestDatabase1);
    TEST2(err, KErrNone);
    
    //3. Check 'temp' folder exists
    err = TheFs.MkDir(KServerTempDir);
    TEST2(err, KErrAlreadyExists);
    }	
	
void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-4154 DEF143062: SQL, \"CREATE INDEX\" sql crashes SQL server"));
	DEF143062();

    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-4155 DEF143061: SQL, SQLITE_DEFAULT_JOURNAL_SIZE_LIMIT value is too big"));
    DEF143061();

    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-4156 DEF143150: SQL, strftime() returns incorrect result"));
    DEF143150();
    
    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4210 Temp files created during sql operations are not deleted after rebooting the phone - 1"));
    DeleteTempFile();
    
    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4211 Temp files created during sql operations are not deleted after rebooting the phone - 2"));
    TempFileTest();
    
    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4213 No support to store an empty string in symbian's sqlite."));
    EmptyTextColumnTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4214 After *#7370# Java apps are not preinstalled again"));
    DeleteTempFolder();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	CreateTestEnv();
	TRAPD(err, DoTestsL());
	DestroyTestEnv();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
