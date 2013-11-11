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

RTest TheTest(_L("t_sqlfilesrvcrash1 test"));

#if !defined __WINS__ && !defined __WINSCW__

RFs TheFs;
RSqlDatabase TheDb;

const TInt KControlIoRuggedOn=2;
const TInt KControlIoRuggedOff=3;
const TInt KControlIoIsRugged=4;

const TChar KDrvLetter = ('E');
_LIT(KTestDir, "E:\\test\\");
_LIT(KDbName, "E:\\test\\t_sqlfilesrvcrash.db");
_LIT8(KConfigStr, "page_size=32768");
_LIT(KFileSrvName, "efile.exe");

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
        DeleteTestFiles();
        RDebug::Print(_L("*** Line %d\r\n"), aLine);
        TheTest(EFalse, aLine);
        }
    }
void Check(TInt aValue, TInt aExpected, TInt aLine)
    {
    if(aValue != aExpected)
        {
        DeleteTestFiles();
        RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
        TheTest(EFalse, aLine);
        }
    }

#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
//Set the drive to Rugged or non-rugged depending on the input parameter
void SetDrive(TBool aSetToRugged)
    {
    TUint8 isRugged;
    TPtr8 pRugged(&isRugged,1,1);

    TInt drvNum;
    TInt err = TheFs.CharToDrive(KDrvLetter, drvNum);
    TEST2(err, KErrNone);
    err=TheFs.ControlIo(drvNum,KControlIoIsRugged,pRugged);
    TEST2(err, KErrNone);
    
    if(!aSetToRugged)
        {
        if(isRugged)
            {
            err=TheFs.ControlIo(drvNum,KControlIoRuggedOff);
            TEST2(err, KErrNone);
            }
        }
    else
        {
        if(!isRugged)
            {
            err=TheFs.ControlIo(drvNum,KControlIoRuggedOn);
            TEST2(err, KErrNone);
            }
        }
    }

//Creates file session instance and the test directory
void CreateTestEnv()
    {
    TInt err = TheFs.Connect();
    TEST2(err, KErrNone);
    
    //Set the drive to non-rugged
    TBool setDriveToRugged = EFalse;
    SetDrive(setDriveToRugged);
    
    err = TheFs.MkDir(KTestDir);
    TEST(err == KErrNone || err == KErrAlreadyExists);
    }

TInt KillProcess(const TDesC& aProcessName)
    {
    TFullName name;
    //RDebug::Print(_L("Find and kill \"%S\" process.\n"), &aProcessName);
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
                //RDebug::Print(_L(":: Process name: \"%S\".\n"), &name);
                continue;
                }
            }
        RProcess proc;
        if (proc.Open(name) == KErrNone)
            {
            RDebug::Print(_L("About to kill process \"%S\", This will force a reboot\n"), &name);
            proc.Kill(0);
            
            }
        proc.Close();
        }
    return KErrNone;
    }

/**
@SYMTestCaseID          PDS-SQL-CT-4164
@SYMTestCaseDesc        Tests for DEF144027: SQL Open returns error if the reported and actual file size are different
                        This function creates a corrupted jorunal file on a non-rugged FAT, where the actual and 
                        reported file size is different. This is done by killing the file server forcing the device to 
                        reset. After the reset t_sqlfilesrvcrash2.exe is used to verify SQL can handle this corrupted
                        journal file properly.
@SYMTestActions         DEF144027: SQL Open returns error if the reported and actual file size are different
@SYMTestExpectedResults Test must not fail
@SYMTestPriority        Medium
@SYMDEF                 DEF144027
                        DEF144238
*/
void DEF144027()
	{
    TInt err = TheDb.Create(KDbName, &KConfigStr);
    TEST2(err, KErrNone);
    
    err = TheDb.Exec(_L("CREATE TABLE t1(NUM INTEGER)"));
    TEST2(err, 1);
    err = TheDb.Exec(_L("CREATE TABLE t2(NUM INTEGER)"));
    TEST2(err, 1);
    
    err = TheDb.Exec(_L("INSERT INTO t1(NUM) VALUES (1)"));
    TEST2(err, 1);
    
    err = TheDb.Exec(_L("INSERT INTO t2(NUM) VALUES (1)"));
    TEST2(err, 1);
    
    TheDb.Close();
    
    err = TheDb.Open(KDbName);
    TEST2(err, KErrNone);
    
    err = TheDb.Exec(_L("BEGIN"));
    TEST(err >= 0);
    
    err = TheDb.Exec(_L("CREATE TABLE t3(NUM INTEGER)"));
    TEST2(err, KErrNone);
    
    err = TheDb.Exec(_L("INSERT INTO t1(NUM) VALUES (1)"));
    TEST2(err, 1);
    
    err = TheDb.Exec(_L("INSERT INTO t2(NUM) VALUES (1)"));
    TEST2(err, 1);
    
    err = TheDb.Exec(_L("INSERT INTO t3(NUM) VALUES (1)"));
    TEST2(err, 1);
    
   
    //Here the transaction is committed but the database is not close
    //this makes sure that the journal is truncated but not deleted 
    err = TheDb.Exec(_L("COMMIT"));
    TEST(err >= 0);
    
    //Reset the drive to rugged
    TBool setDriveToRugged = ETrue;
    SetDrive(setDriveToRugged);
    
    //When the file server is killed, the file size meta data will not be updated
    KillProcess(KFileSrvName);  
	}

void DoTests()
    {    
    TheTest.Start(_L(" @SYMTestCaseID:PDS-SQL-CT-4164 DEF144027: SQL Open returns error if the reported and actual file size are different"));
    DEF144027();    
    }
#endif //#if !defined __WINS__ && !defined __WINSCW__

TInt E32Main()
    {
    TheTest.Title();
    
    CTrapCleanup* tc = CTrapCleanup::New();
    
    __UHEAP_MARK;
    
#if !defined __WINS__ && !defined __WINSCW__
    CreateTestEnv();
    DeleteTestFiles();
    DoTests();
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
