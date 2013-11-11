// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "SqlSrvMain.h"
#include "SqlSrvStartup.h"
#include "SqlSrvUtil.h"

_LIT(KCfgDb1ConfigFilePath, "c:\\private\\10281e17\\cfg[10281E17]t_sqlstartup1.db.02"); // config file version 2 for t_sqlstartup1.db
_LIT(KCfgDb2ConfigFilePath, "c:\\private\\10281e17\\cfg[10281E17]t_sqlstartup2.db.05"); // config file version 5 for t_sqlstartup2.db

//This subdir is created by t_sqlenvcreate app. It should not be returned in the list of files for backup.
_LIT(KPrivateSubDir, "c:\\private\\10281e17\\TestDir.db");

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlstartup test"));

RFs TheFs;

static TInt TheProcessHandleCount = 0;
static TInt TheThreadHandleCount = 0;
static TInt TheAllocatedCellsCount = 0;

#ifdef _DEBUG
static const TInt KBurstRate = 20;
#endif

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	(void)TheFs.Delete(KCfgDb2ConfigFilePath);
	(void)TheFs.Delete(KCfgDb1ConfigFilePath);
	TheFs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Expresssion evaluated to false\r\n"));
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MarkHandles()
    {
    RThread().HandleCount(TheProcessHandleCount, TheThreadHandleCount);
    }

static void MarkAllocatedCells()
    {
    TheAllocatedCellsCount = User::CountAllocCells();
    }

static void CheckAllocatedCells()
    {
    TInt allocatedCellsCount = User::CountAllocCells();
    TEST2(allocatedCellsCount, TheAllocatedCellsCount);
    }

static void CheckHandles()
    {
    TInt endProcessHandleCount;
    TInt endThreadHandleCount;
    
    RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

    TEST2(TheProcessHandleCount, endProcessHandleCount);
    TEST2(TheThreadHandleCount, endThreadHandleCount);
    }

static void OomPreStep(TInt
#ifdef _DEBUG        
    aFailingAllocationNo
#endif
                      )
    {
    MarkHandles();
    MarkAllocatedCells();
    __UHEAP_MARK;
    __UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, aFailingAllocationNo, KBurstRate);
    }

static void OomPostStep()
    {
    __UHEAP_RESET;
    __UHEAP_MARKEND;
    CheckAllocatedCells();
    CheckHandles();
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void CreateAndDestroySqlServerL()
    {
    CSqlServer* server = CSqlServer::NewLC();
    //Drive C: to the RSqlDriveSpaceCol object. This will allow "reserve drive space" construct/destroy code to be tested.  
    RSqlDriveSpaceCol& drvcol = server->DriveSpaceCol();
    drvcol.AddL(EDriveC);
    CleanupStack::PopAndDestroy(server);
    }

static CSqlServer* CreateSqlServerL()
    {
    CSqlServer* server = CSqlServer::NewLC();
    CleanupStack::Pop(server);
    return server;
    }

/**
@SYMTestCaseID          PDS-SQL-UT-4159
@SYMTestCaseDesc        SQL server startup OOM test
@SYMTestPriority        High
@SYMTestActions         Runs the SQL server startup code in an OOM loop.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144096
*/  
void SqlServerStartupOomTest()
    {
    TInt err = KErrNoMemory;
    TInt failingAllocationNo = 0;
    TheTest.Printf(_L("Iteration:\r\n"));
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo);
        TRAP(err, CreateAndDestroySqlServerL());
        OomPostStep();
        }
    if(err != KErrNoMemory)
        {
        TEST2(err, KErrNone);   
        }
    TheTest.Printf(_L("\r\n===OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-SQL-UT-4160
@SYMTestCaseDesc        CSqlServer::GetBackUpListL() OOM test
@SYMTestPriority        High
@SYMTestActions         Calls CSqlServer::GetBackUpListL() in an OOM loop.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144096
*/  
void GetBackupListOomTest()
    {
    CSqlServer* server = NULL;
    TRAPD(err, server = CreateSqlServerL());
    TEST2(err, KErrNone);
  
    TInt fileCnt = 0;
    err = KErrNoMemory;
    TInt failingAllocationNo = 0;
    TheTest.Printf(_L("Iteration:\r\n"));
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo);
        const TUid KDbUd = {0x98765432};
        RArray<HBufC*> files;
        TRAP(err, server->GetBackUpListL(KDbUd, EDriveC, files));
        fileCnt = files.Count();
        if(err == KErrNone)
        	{
			//No directories should be returned in the list of files for backup
			for(TInt i=0;i<fileCnt;++i)
				{
				TPtrC fname = files[i]->Des();
				TInt rc = KPrivateSubDir().CompareF(fname);
				TEST(rc != 0);
				}
        	}
        for(TInt j=0;j<files.Count();++j)
        	{
			delete files[j];
        	}
        files.Close();
        OomPostStep();
        }
    
    delete server;
    
    if(err != KErrNoMemory)
        {
        TEST2(err, KErrNone);   
        }
    TheTest.Printf(_L("\r\n===OOM test succeeded at heap failure rate of %d ===\r\nFile count: %d\r\n"), failingAllocationNo, fileCnt);
    }

/**
@SYMTestCaseID          PDS-SQL-UT-4161
@SYMTestCaseDesc        SQL server startup file I/O error simulation test
@SYMTestPriority        High
@SYMTestActions         Runs the SQL server startup code in a file I/O error simulation loop.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144096
*/  
void SqlServerStartupFileIoErrorTest()
    {
    RFs fs;
    TInt err = fs.Connect();
    TEST2(err, KErrNone);
    
    for(TInt fsError=KErrNotFound;fsError>=KErrBadName;--fsError)
        {
        TheTest.Printf(_L("===Simulated error: %d\r\nIteration: "), fsError);
        err = KErrNotFound;
        TInt cnt=0;
        while(err<KErrNone)
            {
            TheTest.Printf(_L("%d "), cnt);
            (void)fs.SetErrorCondition(fsError, cnt);
            TRAP(err, CreateAndDestroySqlServerL());
            (void)fs.SetErrorCondition(KErrNone);
            if(err != KErrNone)
                {
                ++cnt;
                }
            }
        TEST2(err, KErrNone);
        TheTest.Printf(_L("\r\n===File I/O error simulation test succeeded on iteration %d===\r\n"), cnt);
        }

    fs.Close();
    }

/**
@SYMTestCaseID          PDS-SQL-UT-4162
@SYMTestCaseDesc        CSqlServer::GetBackUpListL() file I/O error simulation test
@SYMTestPriority        High
@SYMTestActions         Calls CSqlServer::GetBackUpListL() in a file I/O error simulation loop.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144096
*/  
void GetBackupListFileIoErrorTest()
    {
    CSqlServer* server = NULL;
    TRAPD(err, server = CreateSqlServerL());
    TEST2(err, KErrNone);

    for(TInt fsError=KErrNotFound;fsError>=KErrBadName;--fsError)
        {
        TheTest.Printf(_L("===Simulated error: %d\r\nIteration: "), fsError);
        err = KErrNotFound;
        TInt fileCnt = 0;
        TInt cnt=0;
        while(err<KErrNone)
            {
            TheTest.Printf(_L("%d "), cnt);
            (void)server->Fs().SetErrorCondition(fsError, cnt);
            const TUid KDbUd = {0x98765432};
            RArray<HBufC*> files;
            TRAP(err, server->GetBackUpListL(KDbUd, EDriveC, files));
            fileCnt = files.Count(); 
            if(err == KErrNone)
            	{
    			//No directories should be returned in the list of files for backup
    			for(TInt i=0;i<fileCnt;++i)
    				{
    				TPtrC fname = files[i]->Des();
    				TInt rc = KPrivateSubDir().CompareF(fname);
    				TEST(rc != 0);
    				}
            	}
            for(TInt j=0;j<files.Count();++j)
            	{
				delete files[j];
            	}
            files.Close();
            (void)server->Fs().SetErrorCondition(KErrNone);
            if(err != KErrNone)
                {
                ++cnt;
                }
            }
        TEST2(err, KErrNone);
        TheTest.Printf(_L("\r\n===File I/O error simulation test succeeded on iteration %d===\r\nFile count: %d\r\n"), cnt, fileCnt);
        }
        
    delete server;
    }

/**
@SYMTestCaseID          PDS-SQL-UT-4224
@SYMTestCaseDesc        CSqlServer::GetBackUpListL() functional test
@SYMTestPriority        High
@SYMTestActions         Calls CSqlServer::GetBackUpListL() and tests the output, when the drive is read-only,
                        when there is a sub-directory which name is matching the search pattern.
@SYMTestExpectedResults Test must not fail
*/  
void GetBackupListFunctionalTest()
	{
    CSqlServer* server = NULL;
    TRAPD(err, server = CreateSqlServerL());
    TEST2(err, KErrNone);
    //Case 1: database with specified uid bellow do exist (on drive C). There will be one subdirectory matching the search pattern. 
    const TDriveNumber KTestDrvNum1 = EDriveC;
    const TUid KDbUid = {0x98765432};
	TDriveUnit testDrive(KTestDrvNum1);
	TDriveName testDriveName = testDrive.Name();
	testDriveName.LowerCase(); 
	//One test directory will be created, which name will be matching the search pattern.
	//The directory name should not be included in the list with the file names.
    TFileName testFileName;
    err = server->Fs().PrivatePath(testFileName);
    TEST2(err, KErrNone);
    testFileName.Append(KDbUid.Name());
    _LIT(KTestPath, "t_startup\\");
    testFileName.Append(KTestPath);
    testFileName.Append(_L("t_startup.db"));
    TParse parse;
    err = parse.Set(testFileName, &testDriveName, 0);
    TEST2(err, KErrNone);
    err = server->Fs().MkDirAll(parse.FullName());
    TEST(err == KErrNone || err == KErrAlreadyExists);
    //
    RArray<HBufC*> files;
    TRAP(err, server->GetBackUpListL(KDbUid, KTestDrvNum1, files));
    TEST2(err, KErrNone);
    TInt fileCnt = files.Count();
    for(TInt i=0;i<fileCnt;++i)
    	{
		TPtrC fname = files[i]->Des();
		TheTest.Printf(_L("Db: %S\r\n"), &fname);
		TEST(fname.FindF(KTestPath) < 0);
		//The name should include the full path + the drive
		err = parse.Set(fname, 0, 0);
		TEST2(err, KErrNone);
		TEST(parse.DrivePresent());
		TEST(parse.PathPresent());
		TDriveName driveName(parse.Drive());
		driveName.LowerCase(); 
		delete files[i];
		TEST(driveName == testDriveName);		
    	}
    files.Close();
    //Case 2: drive Z:. No files should be returned.
    const TDriveNumber KTestDrvNum2 = EDriveZ;
    TRAP(err, server->GetBackUpListL(KDbUid, KTestDrvNum2, files));
    TEST2(err, KErrNone);
    fileCnt = files.Count();
    TEST2(fileCnt, 0);
    //Case 3: drive A:. The drive does not exist. No files should be returned.
    const TDriveNumber KTestDrvNum3 = EDriveA;
    TRAP(err, server->GetBackUpListL(KDbUid, KTestDrvNum3, files));
	TheTest.Printf(_L("Drive %d, err=%d\r\n"), KTestDrvNum3, err);
    fileCnt = files.Count();
    TEST2(fileCnt, 0);
    //
    delete server;
	}

/**
@SYMTestCaseID          PDS-SQL-UT-4163
@SYMTestCaseDesc        Test for DEF144196: SQL, server code coverage can be improved
@SYMTestPriority        High
@SYMTestActions         Tests the UTF conversion functions implemented in SqlSrvUtil.cpp.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144196
*/  
void UtfConversionTest()
    {
    /////////       UTF16ToUTF8()       ///////////////////////
    _LIT(KStr16, "abcd");
    _LIT8(KStr8, "abcd");
    TBuf8<KMaxFileName + 1> bufout;
    TBool rc = UTF16ToUTF8(KStr16, bufout);
    TEST(rc);
    TEST(bufout == KStr8);
    //Test where the input buffer contains non-convertible characters
    TBuf<2> name2;
    name2.SetLength(2);
    name2[0] = TChar(0xD800); 
    name2[1] = TChar(0xFC00); 
    rc = UTF16ToUTF8(name2, bufout);
    TEST(!rc);
    /////////       UTF16ToUTF8Z()       ///////////////////////
    _LIT8(KStr8Z, "abcd\x0");
    rc = UTF16ToUTF8Z(KStr16, bufout);
    TEST(rc);
    TEST(bufout == KStr8Z);
    //Test where the input buffer contains non-convertible characters
    rc = UTF16ToUTF8Z(name2, bufout);
    TEST(!rc);
    /////////       UTF16ZToUTF8Z()       ///////////////////////
    _LIT(KStr16Z, "abcd\x0");
    rc = UTF16ZToUTF8Z(KStr16Z, bufout);
    TEST(rc);
    TEST(bufout == KStr8Z);
    //Test where the input buffer contains non-convertible characters
    TBuf<3> name3;
    name3.SetLength(3);
    name3[0] = TChar(0xD800); 
    name3[1] = TChar(0xFC00); 
    name3[2] = TChar(0x0000); 
    rc = UTF16ZToUTF8Z(name3, bufout);
    TEST(!rc);
    }

/**
@SYMTestCaseID          PDS-SQL-UT-4175
@SYMTestCaseDesc        Test for DEF144937: SQL, SQL server, the code coverage can be improved in some areas. 
@SYMTestPriority        High
@SYMTestActions         The test creates a SQL server instance and performs some basic operations with
                        RSqlDriveSpaceCol object owned by the server, such as: adding a new drive,
                        getting an access to the reserved drive space, releasing the access.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144937
*/  
void ReserveDriveSpaceTest()
    {
    CSqlServer* srv = NULL;
    TRAPD(err, srv = CreateSqlServerL());
    TEST2(err, KErrNone);
    
    RSqlDriveSpaceCol& drvcol = srv->DriveSpaceCol();
    TRAP(err, drvcol.AddL(EDriveC));
    TEST2(err, KErrNone);

    CSqlDriveSpace* drvspace = drvcol.Find(EDriveZ);
    TEST(!drvspace);
    drvspace = drvcol.Find(EDriveC);
    TEST(drvspace != NULL);
    
    TDriveNumber drvnum = drvspace->Drive();
    TEST2(drvnum, EDriveC);
    //It is safe to call GetAccessL() more than once. The access is reference counted.
    TRAP(err, drvspace->GetAccessL());
    TEST2(err, KErrNone);
    TRAP(err, drvspace->GetAccessL());
    TEST2(err, KErrNone);
    //It is safe if ReleaseAccess() call count do not match GetAccessL() call count.
    drvspace->ReleaseAccess();
    drvspace->ReleaseAccess();
    drvspace->ReleaseAccess();
    //
    drvcol.ResetAndDestroy();
    delete srv;
    }

void DoCreateCfgFile(const TDesC& aFileName, const TDesC8& aData)
    {
    RFile file;
    TInt err = file.Create(TheFs, aFileName, EFileRead | EFileWrite);
    TEST2(err, KErrNone);
    err = file.Write(aData); 
    file.Close();   
    TEST2(err, KErrNone);
    }

void DoTests()
	{
    CActiveScheduler* scheduler = new CActiveScheduler;
    TEST(scheduler != NULL);
    CActiveScheduler::Install(scheduler);

    //Adding two db config files will allow CDbConfigFiles construct/destroy code also to be tested in the OOM tests.
    TInt err = TheFs.Connect();
    TEST2(err, KErrNone);
    DoCreateCfgFile(KCfgDb1ConfigFilePath, _L8("CREATE INDEX idx ON table1(i1);"));
    DoCreateCfgFile(KCfgDb2ConfigFilePath, _L8("CREATE INDEX idx1 ON table1(i1);CREATE INDEX idx2 ON table2(i2)"));
    
    TheTest.Start(_L(" @SYMTestCaseID:PDS-SQL-UT-4159 SQL server startup OOM test"));
    SqlServerStartupOomTest();

    TheTest.Next (_L(" @SYMTestCaseID:PDS-SQL-UT-4160 CSqlServer::GetBackUpListL() OOM test"));
    GetBackupListOomTest();

    TheTest.Next (_L(" @SYMTestCaseID:PDS-SQL-UT-4161 SQL server startup file I/O error simulation test"));
    SqlServerStartupFileIoErrorTest();
    
    TheTest.Next (_L(" @SYMTestCaseID:PDS-SQL-UT-4162 CSqlServer::GetBackUpListL() file I/O error simulation test"));
    GetBackupListFileIoErrorTest();

    TheTest.Next (_L(" @SYMTestCaseID:PDS-SQL-UT-4224 CSqlServer::GetBackUpListL() functional test"));
    GetBackupListFunctionalTest();
    
    TheTest.Next (_L(" @SYMTestCaseID:PDS-SQL-UT-4163 SQL server, UTF conversion test"));
    UtfConversionTest();

    TheTest.Next (_L(" @SYMTestCaseID:PDS-SQL-UT-4175 Reserve drive space tests"));
    ReserveDriveSpaceTest();
    
    delete scheduler;
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;
	
	DoTests();
	DeleteTestFiles();

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
