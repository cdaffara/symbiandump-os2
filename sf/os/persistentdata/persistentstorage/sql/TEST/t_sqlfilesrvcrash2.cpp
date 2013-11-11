// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
Instructions:

This is a manual test created to verify DEFXXXXX, 2 executables needs to be run to do this test
1) t_sqlfilesrvcrash1.exe - Generate a corrupted journal file, this will cause the device to reset. 
2) t_sqlfilesrvcrash2.exe - After the reboot, tests if SQL can handle the courrpted journal file.

This test requires a non-rugged drive to store the database file and therefore will only work in hardware mode
*/

#include <e32test.h>
#include <f32file.h>
#include <sqldb.h>

RTest TheTest(_L("t_sqlfilesrvcrash2 test"));

#if !defined __WINS__ && !defined __WINSCW__

RFs TheFs;
RSqlDatabase TheDb;

_LIT(KDbName, "E:\\test\\t_sqlfilesrvcrash.db");
_LIT(KJournalName,  "E:\\test\t_sqlfilesrvcrash.db-journal");
///////////////////////////////////////////////////////////////////////////////////////
//Deletes all created test files.
void DeleteTestFiles()
    {
    TheDb.Close();
    (void)RSqlDatabase::Delete(KDbName);
    }

///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
    {
    if(!aValue)
        {
        RDebug::Print(_L("*** Line %d\r\n"), aLine);
        TheTest(EFalse, aLine);
        }
    }
void Check(TInt aValue, TInt aExpected, TInt aLine)
    {
    if(aValue != aExpected)
        {
        RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
        TheTest(EFalse, aLine);
        }
    }

#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)
///////////////////////////////////////////////////////////////////////////////////////
//Creates file session instance and the test directory
void CreateTestEnv()
    {
    TInt  err = TheFs.Connect();
    TEST2(err, KErrNone);
    
    RFile file;
    err = file.Open(TheFs, KJournalName, EFileRead);
    TEST2(err, KErrNone);
    
    TInt size;
    err = file.Size(size);
    TEST2(err, KErrNone);
    TEST(size > SQLITE_DEFAULT_JOURNAL_SIZE_LIMIT);
    file.Close();
    }
///////////////////////////////////////////////////////////////////////////////////////
/**
@SYMTestCaseID          PDS-SQL-CT-4165
@SYMTestCaseDesc        Tests for DEF144027: SQL Open returns error if the reported and actual file size are different
                        Requires a corrupted journal file to be created using t_sqlfilesrvcrash1.exe before running 
                        this test. If a corrupted journal file exists then check that the opening the database does not
                        return an error.
@SYMTestActions         DEF144027: SQL Open returns error if the reported and actual file size are different
@SYMTestExpectedResults The RSqlDatabase::Open operation should not fail
@SYMTestPriority        Medium
@SYMDEF                 DEF144027
                        DEF144238
*/
void DEF144027()
	{
    TInt err = TheDb.Open(KDbName);
    TEST2(err, KErrNone);
    
    //Lets perform a simple operation to make sure it works
    err = TheDb.Exec(_L("BEGIN"));
    TEST(err >= 0);
    
    err = TheDb.Exec(_L("INSERT INTO t1(NUM) VALUES (55)"));
    TEST2(err, 1);
    
    err = TheDb.Exec(_L("INSERT INTO t2(NUM) VALUES (55)"));
    TEST2(err, 1);
        
    err = TheDb.Exec(_L("COMMIT"));
    TEST(err >= 0);
        
    TheDb.Close();
	}

void DoTests()
    {    
    TheTest.Start(_L(" @SYMTestCaseID:PDS-SQL-CT-4165 DEF144027: SQL Open returns error if the reported and actual file size are different"));
    DEF144027();    
    }
#endif //#if !defined __WINS__ && !defined __WINSCW__

TInt E32Main()
    {
    TheTest.Title();
    
    CTrapCleanup* tc = CTrapCleanup::New();
    
    __UHEAP_MARK;
    
#if !defined __WINS__ && !defined __WINSCW__
    DoTests();
    DeleteTestFiles();
    TheFs.Close();
    TheTest.End();
#else
    TheTest.Start(_L("This test works only works on hardware!"));
    TheTest.End();
#endif  
 
    __UHEAP_MARKEND;
    
    TheTest.Close();
    
    delete tc;

    User::Heap().Check();
    return KErrNone;
    }
