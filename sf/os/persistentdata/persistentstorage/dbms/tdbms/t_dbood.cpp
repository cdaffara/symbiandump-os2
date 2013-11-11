// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Testing new RDbs methods, which handle "Out of disk space" situations.
// 
//

#include <e32test.h>
#include <f32file.h>
#include <d32dbms.h>

/////////////////////////////////////////////////////////////////
//Globals

//If you change KTestDrive, don't forget to change KTestDatabase too!!!

#if  defined __WINSCW__ || defined __WINS__

	//The C: drive may be too big and may be used concurently by other applications. 
	//The T: drive is more suitable for the test if running on the emulator
	const TInt				KTestDrive = EDriveT;
	_LIT(					KTestDatabase, "T:\\DBMS-TST\\T_DbmsOOD.DB");
	
#elif defined __X86GCC__

	const TInt				KTestDrive = EDriveG;
	_LIT(					KTestDatabase, "G:\\DBMS-TST\\T_DbmsOOD.DB");
	
#else

	const TInt				KTestDrive = EDriveE;
	_LIT(					KTestDatabase, "E:\\DBMS-TST\\T_DbmsOOD.DB");
	
#endif

const TInt				KReservedSpaceSize = 0; //The aSpace parameter of RDbs::ReserveDriveSpace()
                                                //is not used at the moment and shall be set to 0.

static RTest			TheTest(_L("t_dbood - \"Out of Disk space\" test"));
static RFs				TheFs;
static RDbNamedDatabase TheDb;
static RDbs				TheDbSession;

//Test table defs
_LIT(KTestTableName, "TABLE1");

const TInt KTestRecordsCount = 350;

struct TColDef
	{
	const TText*	iName;
	TDbColType		iType;
	TInt			iAttributes;
	};
static TColDef const KColDefs[]=
	{
		{_S("ID"), EDbColUint32, TDbCol::EAutoIncrement},
		{_S("DATA"), EDbColBinary, TDbCol::ENotNull},
		{0}
	};

//One or more files with KLargeFileName name and ".<n>" extension, where n is
//000, 001, 002, 003...
//will be created and they will occupy almost all available disk space.
//The idea is to perform after that "delete"
//transaction, which has to fail, because of "out of disk space" condition.
#if  defined __WINSCW__ || defined __WINS__

	_LIT(KLargeFileName, "T:\\DBMS-TST\\DeleteMe");

#elif defined  __X86GCC__

	_LIT(KLargeFileName, "G:\\DBMS-TST\\DeleteMe");

#else

	_LIT(KLargeFileName, "E:\\DBMS-TST\\DeleteMe");

#endif

static void AssembleLargeFileName(const TDesC& aFileName, TInt aFileNumber, TDes& aResultPath)
	{
	_LIT(KFormatStr, "%S.%03d");
	aResultPath.Format(KFormatStr, &aFileName, aFileNumber);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Create/Destroy test environment - global functions

//Deletes "aFullName" file.
static TInt DeleteDataFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		err = fsSession.Entry(aFullName, entry);
		if(err == KErrNone)
			{
			RDebug::Print(_L("Deleting \"%S\" file.\n"), &aFullName);
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
				}
			err = fsSession.Delete(aFullName);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	return err;
	}

//Deletes large data files only
static void DeleteLargeDataFiles()
	{
	for(TInt i=0;i<1000;++i)
		{
		TBuf<KMaxFileName> filePath;
		AssembleLargeFileName(KLargeFileName, i, filePath);
		if(DeleteDataFile(filePath) != KErrNone)
			{
			break;
			}
		}
	}

//Deletes data files used by the test
static void DeleteDataFiles()
	{
	if(TheDbSession.Handle())
		{
		TheDb.Close();
		}
	TheDbSession.Close();
	DeleteDataFile(KTestDatabase);
	DeleteLargeDataFiles();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Tests macros and functions.
//If (!aValue) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteDataFiles();
		TheTest(EFalse, aLine);
		}
	}
//If (aValue != aExpected) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		DeleteDataFiles();
		TheTest(EFalse, aLine);
		}
	}
//Use these to test conditions.
#define TEST(arg) Check((arg), __LINE__)
#define TEST2(aValue, aExpected) Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Global functions

//Prepares the test directory.
//TheFs.Connect() has to be called already.
static void SetupTestDirectory()
    {
	TInt err = TheFs.MkDir(KTestDatabase);
	if(err != KErrNone)
	    {
	    RDebug::Print(_L("*** SetupTestDirectory(), RFs::MkDir(), err=%d\r\n"), err);
	    }
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

//Leaves with info message printed out
static void LeaveL(TInt aError, TInt aLine)
	{
	RDebug::Print(_L("*** Leave. Error: %d, Line: %d\r\n"), aError, aLine);
	User::Leave(aError);
	}

//Leaves if aError < 0 with info message printed out
static void LeaveIfErrorL(TInt aError, TInt aLine)
	{
	if(aError < KErrNone)
		{
		LeaveL(aError, aLine);
		}
	}

//Use LEAVE() macro instead of User::Leave() and LEAVE_IF_ERROR() macro instead of
//User::LeaveIfError(). They will print the line number, where the "leave" was called.
#define LEAVE(aError) LeaveL(aError, __LINE__)
#define LEAVE_IF_ERROR(aError) LeaveIfErrorL(aError, __LINE__)

//Creates one or more large files with the total size near to the size of the available disk space.
//The idea is to cause  an "out of disk space" condition.
static void FillLargeDataFileL(RFile& aFile, TInt aSize)
	{
    TInt err = KErrDiskFull;
    while(err == KErrDiskFull)
        {
        err = aFile.SetSize(aSize);
        aSize -= 100;
        if(aSize <= 0)
            {
            break;
            }
        }
    TEST(err == KErrNone || err == KErrDiskFull);
	}

//Gets the available space of the tested drive.
static TInt64 FreeDiskSpaceL()
	{
	TVolumeInfo volInfoBefore;
	LEAVE_IF_ERROR(TheFs.Volume(volInfoBefore, KTestDrive));
	return volInfoBefore.iFree;
	}

//Creates large data file with aSize size (in bytes).
static void DoCreateLargeFileL(const TDesC& aPath, TInt aSize)
	{
	RFile file;
	CleanupClosePushL(file);
	LEAVE_IF_ERROR(file.Replace(TheFs, aPath, EFileRead | EFileWrite));
	FillLargeDataFileL(file, aSize);
	LEAVE_IF_ERROR(file.Flush());
	CleanupStack::PopAndDestroy(&file);
	}

//Creates enough number of large data files to fill the available disk space.
//It will change FilesCount global variable's value.
static void CreateLargeFileL()
	{
	TInt fileNo = 0;
	const TInt KLargeFileSize = 1000000000;
	TInt64 diskSpace = FreeDiskSpaceL();
	RDebug::Print(_L("CreateLargeFileL: free space before = %ld\n"), diskSpace);
	TBuf<KMaxFileName> filePath;
    const TInt64 KMinDiskSpace = 200;
	//Reserve almost all disk space, except a small amount - 200 bytes.
	while(diskSpace > KMinDiskSpace)
		{
		AssembleLargeFileName(KLargeFileName, fileNo++, filePath);
		TInt fileSize = KLargeFileSize;
        if(diskSpace < (TInt64)KLargeFileSize)
            {
		    TInt64 lastFileSize = diskSpace - KMinDiskSpace;
            fileSize = I64LOW(lastFileSize);
            }
		DoCreateLargeFileL(filePath, fileSize);
		diskSpace = FreeDiskSpaceL();
		RDebug::Print(_L("----CreateLargeFileL, step %d, free space = %ld\n"), fileNo, diskSpace);
		}
	diskSpace = FreeDiskSpaceL();
	RDebug::Print(_L("CreateLargeFileL: free space after = %ld\n"), diskSpace);
	}

//Reserves disk space for TheDbSession instance.
//TheDbSession instance has to be connected already.
static void ReserveDiskSpace()
	{
	TInt err = TheDbSession.ReserveDriveSpace(KTestDrive, KReservedSpaceSize);
	TEST2(err, KErrNone);
	}

//Frees already reserved disk space for TheDbSession instance.
//TheDbSession instance has to be connected already.
static void FreeReservedSpace()
	{
	TheDbSession.FreeReservedSpace(KTestDrive);
	}

//Gets an access to the reserved disk space for TheDbSession instance.
//TheDbSession instance has to be connected already.
static void UnlockReservedSpace()
	{
	TInt err = TheDbSession.GetReserveAccess(KTestDrive);
	TEST2(err, KErrNone);
	}

//Releases the access to the reserved disk space.
//TheDbSession instance has to be connected already.
static void LockReservedSpace()
	{
	(void)TheDbSession.ReleaseReserveAccess(KTestDrive);
	}

//Creates the test DBMS session
static void CreateTestDbSession()
	{
	TInt err = TheDbSession.Connect();
	TEST2(err, KErrNone);
	}


//Creates the test database
//TheDbSession instance has to be connected already.
//TheFs.Connect() has to be called already.
static void CreateTestDatabase(RDbs& aDbs, RDbNamedDatabase& aDb)
	{
	//Create the test database.
	TInt err = aDb.Replace(TheFs, KTestDatabase);
	TEST2(err, KErrNone);
	TheDb.Close();
	//Open it now using DBMS session (so, on DBMS server side), because we want to test
	//server side RFs sessions - handling "out of disk space" situations.
	err = aDb.Open(aDbs, KTestDatabase);
	TEST2(err, KErrNone);
	}

//Creates a test table
static void CreateTestTableL(RDbNamedDatabase& aDb)
	{
	CDbColSet* colSet = CDbColSet::NewLC();
	for(const TColDef* colDef=KColDefs;colDef->iName;++colDef)
		{
		TDbCol col(TPtrC(colDef->iName), colDef->iType);
		col.iAttributes = colDef->iAttributes;
		colSet->AddL(col);
		}
	TEST2(aDb.CreateTable(KTestTableName, *colSet), KErrNone);
	CleanupStack::PopAndDestroy(colSet);
	}

//Adds some data to the test table
static void AddTestDataL(RDbNamedDatabase& aDb)
	{
	RDbTable tbl;
	CleanupClosePushL(tbl);
	TEST2(tbl.Open(aDb, KTestTableName, RDbRowSet::EUpdatable), KErrNone);
	for(TInt i=0;i<KTestRecordsCount;++i)
		{
		tbl.InsertL();
		tbl.SetColL(2, _L8("1ABCDEFGHI2ABCDEFGHI3ABCDEFGHI4ABCDEFGHI5ABCDEFGHI6ABCDEFGHI7ABCDEFGHI8ABCDEFGHI9ABCDEFGHI0ABCDEFGHI"));
		tbl.PutL();
		}
	TEST(tbl.CountL() == KTestRecordsCount);
	CleanupStack::PopAndDestroy(&tbl);
	}

//Deletes some records from the test table using "delete" transaction.
//Do not put TEST or TEST2 macro calls here (except for record count checks)!
//The method must leave if some of the calls inside fail.
static void DeleteRecordsL()
	{
	RDbTable tbl;
	CleanupClosePushL(tbl);
	LEAVE_IF_ERROR(tbl.Open(TheDb, KTestTableName, RDbRowSet::EUpdatable));
	TEST(tbl.CountL() == KTestRecordsCount);
	TheDb.Begin();
	tbl.FirstL();
	for(TInt i=0;i<(KTestRecordsCount/2);++i)
		{
		tbl.DeleteL();
		tbl.NextL();
		}
	TInt err = TheDb.Commit();
	if(err != KErrNone)
		{
		TheDb.Rollback();
		LEAVE(err);
		}
	TEST(tbl.CountL() == (KTestRecordsCount / 2));
	CleanupStack::PopAndDestroy(&tbl);
	}

/**
The function simply calls RDbs::ReserveDriveSpace(), RDbs::GetReserveAccess(),
RDbs::ReleaseReserveAccess() methods and checks the return values.
It might be usefull for debugging in case if something gets wrong.

@SYMTestCaseID          SYSLIB-DBMS-CT-0647
@SYMTestCaseDesc        Tests for attempting to reserve disk space
@SYMTestPriority        Medium
@SYMTestActions         Calls up RDbs::ReserveDriveSpace(), RDbs::GetReserveAccess(),
                        RDbs::ReleaseReserveAccess() methods and checks the return values.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void SimpleCallsL()
	{
	RDbs dbs;
	CleanupClosePushL(dbs);
	LEAVE_IF_ERROR(dbs.Connect());

	//Reserve disk space
	TInt err = dbs.ReserveDriveSpace(KTestDrive, KReservedSpaceSize);
	TEST2(err, KErrNone);

	//An attempt to re-reserve it
   	err = dbs.ReserveDriveSpace(KTestDrive, KReservedSpaceSize);
	TEST2(err, KErrInUse);

	//Get an access to the reserved disk space
	err = dbs.GetReserveAccess(KTestDrive);
	TEST2(err, KErrNone);

	//An attempt to get an access to the reserved space twice.
	err = dbs.GetReserveAccess(KTestDrive);
	TEST2(err, KErrInUse);

	//This call must fail, because it tries to get an access to the reserved space of
	//not the same drive, for which ReserveDriveSpace() was called.
	err = dbs.GetReserveAccess(KTestDrive + 1);
	TEST(err != KErrNone);

	(void)dbs.ReleaseReserveAccess(KTestDrive);

	//An attempt to release the reserved space twice. This call will panic in debug mode.
	//(void)dbs.ReleaseReserveAccess(KTestDrive);

	//Cancel reserving an additional disk space
	dbs.FreeReservedSpace(KTestDrive);

	//Cancel reserving an additional disk space twice
    //This call will panic in debug mode.
	//dbs.FreeReservedSpace(KTestDrive);

	CleanupStack::PopAndDestroy(&dbs);
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0648
@SYMTestCaseDesc        Transactions test
						Simulating  an "out of disk space" situation
@SYMTestPriority        Medium
@SYMTestActions         Transaction test under "out of disk space" circumstances
						while reserving disk space.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void TransactionTestL()
	{
    TVolumeIOParamInfo volIoPrm;
    TInt err = TheFs.VolumeIOParam(KTestDrive, volIoPrm);
    TEST2(err, KErrNone);
    RDebug::Print(_L("--Drive %d. BlockSize=%d, ClusterSize=%d, RecReadBufSize=%d, RecWriteBufSize=%d\r\n"), KTestDrive, volIoPrm.iBlockSize, volIoPrm.iClusterSize, volIoPrm.iRecReadBufSize, volIoPrm.iRecWriteBufSize);
    /////////////////////////////////////////////////////////
	CreateTestDbSession();
    //Rserve disk space
	ReserveDiskSpace();
    //Create test database and table. Add some test data to them.
	CreateTestDatabase(TheDbSession, TheDb);
	CreateTestTableL(TheDb);
	AddTestDataL(TheDb);
    RDebug::Print(_L("--Simulate an \"out of disk space\" situation with creating a very large data file, which occupies almost the all the available disk space.\r\n"));
	CreateLargeFileL();
    RDebug::Print(_L("--Attempt to delete test data records. The transaction must fail, because of \"out of disk space\".\r\n"));
    TInt64 diskSpace = FreeDiskSpaceL();
	RDebug::Print(_L("--Attempt to delete test data records. Free disk space = %ld\n"), diskSpace);
	TRAP(err, DeleteRecordsL());
	RDebug::Print(_L("--DeleteRecordsL() returned %d error\r\n"), err);
	TEST(err != KErrNone);
    RDebug::Print(_L("--The attempt failed with err=%d. Get an access to the reserved disk space.\r\n"), err);
    UnlockReservedSpace();
	RDebug::Print(_L("--Try again with getting an access to the reserved disk space.\n"));
    diskSpace = FreeDiskSpaceL();
    RDebug::Print(_L("After GetReserveAccess(), free disk space = %ld\r\n"), diskSpace);
	DeleteRecordsL();
	RDebug::Print(_L("--\"Delete\" transaction was completed successfully.\n"));
    //Free the resources, used in the test
	DeleteLargeDataFiles();
	LockReservedSpace();
    FreeReservedSpace();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0649
@SYMTestCaseDesc        OOD tests with two DBMS sessions.
@SYMTestPriority        Medium
@SYMTestActions         The test actually checks that the DBMS server is in a stable state, when there is more
						than one RDbs session and a shared database is accessed.
						The first check is that the shared database can be accessed without any problem through
						any of the sessions: first or second.
						Then the second session is closed and the shared database is accessed
						through the first DBMS session - the operations should not fail.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void TwoSessTestL()
    {
    //Create session1, open a shared database, open a shared table through session 1
    RDbs dbSess1;
    CleanupClosePushL(dbSess1);
    LEAVE_IF_ERROR(dbSess1.Connect());

    RDbNamedDatabase db1;
    CleanupClosePushL(db1);
	TInt err = db1.Open(dbSess1, KTestDatabase);
	TEST2(err, KErrNone);

	RDbTable tbl1;
	CleanupClosePushL(tbl1);
	TEST2(tbl1.Open(db1, KTestTableName, RDbRowSet::EUpdatable), KErrNone);

    //Create session2, open shared database, open shared table through session 2
    RDbs dbSess2;
    CleanupClosePushL(dbSess2);
    LEAVE_IF_ERROR(dbSess2.Connect());

    RDbNamedDatabase db2;
    CleanupClosePushL(db2);
	err = db2.Open(dbSess2, KTestDatabase);
	TEST2(err, KErrNone);

	RDbTable tbl2;
	CleanupClosePushL(tbl2);
	TEST2(tbl2.Open(db2, KTestTableName, RDbRowSet::EUpdatable), KErrNone);

    //Here we have two sessions and two instances of RDbNamedDatabase type, which
    //operate on a shared database. Insert a record through the sessions.

	tbl1.InsertL();
	tbl1.SetColL(2, _L8("--------------------------1----------------------------------------"));
	tbl1.PutL();

	tbl2.InsertL();
	tbl2.SetColL(2, _L8("========================2======================================"));
	tbl2.PutL();

    //Close the second session. It should be able to access the shared database via the
    //first session.

	CleanupStack::PopAndDestroy(&tbl2);
    CleanupStack::PopAndDestroy(&db2);
    CleanupStack::PopAndDestroy(&dbSess2);

    //Try to access again the shared database.
	tbl1.InsertL();
	tbl1.SetColL(2, _L8("+++++++++++++++++++++++++++++++++++3++++++++++++++++++++++++++++++++++++++++++"));
	tbl1.PutL();

	CleanupStack::PopAndDestroy(&tbl1);
    CleanupStack::PopAndDestroy(&db1);
    CleanupStack::PopAndDestroy(&dbSess1);
    }

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0650
@SYMTestCaseDesc        OOD tests with more than one DBMS session.
@SYMTestPriority        Medium
@SYMTestActions         The test calls ReserveDriveSpace/GetReserveAccess/ReleaseReserveAccess in a different
						combinations on four DBMS sessions. The test should not fail or panic.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void TwoSessTest2L()
    {
    //Create session1
    RDbs dbSess1;
    CleanupClosePushL(dbSess1);
    LEAVE_IF_ERROR(dbSess1.Connect());

    //Create session2
    RDbs dbSess2;
    CleanupClosePushL(dbSess2);
    LEAVE_IF_ERROR(dbSess2.Connect());

    //Play with "ReserveDriveSpace" on both sessions
    TInt err = dbSess1.ReserveDriveSpace(KTestDrive, KReservedSpaceSize);
    TEST2(err, KErrNone);
    err = dbSess2.ReserveDriveSpace(KTestDrive, KReservedSpaceSize);
    TEST2(err, KErrNone);
    dbSess2.FreeReservedSpace(KTestDrive);
    err = dbSess2.ReserveDriveSpace(KTestDrive, KReservedSpaceSize);
    TEST2(err, KErrNone);

    //Get an access to the reserved space through session 2
	err = dbSess2.GetReserveAccess(KTestDrive);
    TEST2(err, KErrNone);
    //Free/re-reserve disk space for session 1.
    dbSess1.FreeReservedSpace(KTestDrive);
    err = dbSess1.ReserveDriveSpace(KTestDrive, KReservedSpaceSize);
    TEST2(err, KErrNone);

    //Create session4
    RDbs dbSess4;
    CleanupClosePushL(dbSess4);
    LEAVE_IF_ERROR(dbSess4.Connect());

    //Try to reserve space for session 4.
    err = dbSess4.ReserveDriveSpace(KTestDrive, KReservedSpaceSize);
    TEST2(err, KErrNone);

    //Create session3
    RDbs dbSess3;
    CleanupClosePushL(dbSess3);
    LEAVE_IF_ERROR(dbSess3.Connect());
    //Try to reserve space for session 3.
    err = dbSess3.ReserveDriveSpace(KTestDrive, KReservedSpaceSize);
    TEST2(err, KErrNone);

    //Release and free session 2 access to the reserved space.
    (void)dbSess2.ReleaseReserveAccess(KTestDrive);
    dbSess2.FreeReservedSpace(KTestDrive);

    dbSess3.FreeReservedSpace(KTestDrive);
    CleanupStack::PopAndDestroy(&dbSess3);

    dbSess4.FreeReservedSpace(KTestDrive);
    CleanupStack::PopAndDestroy(&dbSess4);

    //Get an access to the reserved space through session 2.
    //But it was freed, so the call will fail.
	err = dbSess2.GetReserveAccess(KTestDrive);
    TEST(err != KErrNone);

    //Free/re-reserve disk space for session 1.
    dbSess1.FreeReservedSpace(KTestDrive);
    err = dbSess1.ReserveDriveSpace(KTestDrive, KReservedSpaceSize);
    TEST2(err, KErrNone);

    //Grant/release the access to the reserved space for session 1.
	err = dbSess1.GetReserveAccess(KTestDrive);
    TEST2(err, KErrNone);
    (void)dbSess1.ReleaseReserveAccess(KTestDrive);

    //Grant an access to the reserved space for session 2.
    //The call will fail because there is no reserved disk space for session 2.
	err = dbSess2.GetReserveAccess(KTestDrive);
    TEST(err != KErrNone);

    //Free the reserved space - session 1
    dbSess1.FreeReservedSpace(KTestDrive);

    CleanupStack::PopAndDestroy(&dbSess2);
    CleanupStack::PopAndDestroy(&dbSess1);
    }

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0651
@SYMTestCaseDesc        Out of memory tests
@SYMTestPriority        Medium
@SYMTestActions         Checks RDbs::ReserveDriveSpace() behaviour under OOM circumstances
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void OOMTest1()
    {
    RDbs dbs;
    TEST2(dbs.Connect(), KErrNone);
	dbs.ResourceMark();
	for(TInt count=1;;++count)
		{
        RDebug::Print(_L("OOMTest1. Count=%d\n"), count);
		dbs.SetHeapFailure(RHeap::EFailNext, count);

		TInt ret = dbs.ReserveDriveSpace(KTestDrive, KReservedSpaceSize);

		if(ret == KErrNoMemory)
			{
	        dbs.ResourceCheck();
			}
		else if(ret == KErrNone)
			{
			dbs.FreeReservedSpace(KTestDrive);
			break;
			}
		else
			{
			TEST2(ret, KErrNone);
			}
		}

    dbs.SetHeapFailure(RHeap::ENone, 0);
    dbs.Close();
    }

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0652
@SYMTestCaseDesc        Out of memory tests
@SYMTestPriority        Medium
@SYMTestActions         Checks RDbs::GetReserveAccess() behaviour under OOM circumstances
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void OOMTest2()
    {
    RDbs dbs;
    TEST2(dbs.Connect(), KErrNone);
	TEST2(dbs.ReserveDriveSpace(KTestDrive, KReservedSpaceSize), KErrNone);
	dbs.ResourceMark();
	for(TInt count=1;;++count)
		{
        RDebug::Print(_L("OOMTest2. Count=%d\n"), count);
		dbs.SetHeapFailure(RHeap::EFailNext, count);

		TInt ret = dbs.GetReserveAccess(KTestDrive);

		if(ret == KErrNoMemory)
			{
	        dbs.ResourceCheck();
			}
		else if(ret == KErrNone)
			{
			(void)dbs.ReleaseReserveAccess(KTestDrive);
			break;
			}
		else
			{
			TEST2(ret, KErrNone);
			}
		}

	dbs.FreeReservedSpace(KTestDrive);
    dbs.SetHeapFailure(RHeap::ENone, 0);
    dbs.Close();
    }


//Used in DEF057265().
static TInt ThreadFunc(void*)
	{
	User::SetJustInTime(EFalse);	// disable debugger panic handling
	//Create DBMS session. Reserve drive space.
    RDbs dbs;
    TEST2(dbs.Connect(), KErrNone);
	TEST2(dbs.ReserveDriveSpace(KTestDrive, KReservedSpaceSize), KErrNone);
	//Panic thread. See DBMS server behaviour - will it panic or not?
	//If DBMS server panics in _DEBUG mode - DEF057265 is not properly fixed.
	User::Panic(_L("Simulate DBMS client failuer"), 0);
	return KErrNone;
	}

//DEF057265 - Panics when uninstalling a java midlet while it is running.
//The test will run one thread. Inside the thread's function the test will create
//DBMS session and reserve some disk space. Then the test will panic the thread
//(without freeing the reserved disk space).
//If DBMS server panics in _DEBUG mode - the defect is not fixed.
void DEF057265()
	{
	_LIT(KSessThreadName,"SessThrd");
	RThread sessThread;
	TEST2(sessThread.Create(KSessThreadName, &ThreadFunc, 0x2000, 0, 0), KErrNone);

	TRequestStatus sessThreadStatus;
	sessThread.Logon(sessThreadStatus);
	TEST2(sessThreadStatus.Int(), KRequestPending);

	sessThread.Resume();
	User::WaitForRequest(sessThreadStatus);
	TEST2(sessThread.ExitType(), EExitPanic);

	User::SetJustInTime(EFalse);	// disable debugger panic handling
	sessThread.Close();//This Close() operation will force DBMS server to close
					   //created in ThreadFunc() DBMS session.
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//The main test function.
//Call your new test functions from here
static void RunTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0647 RDbs OOD methods calls "));
	SimpleCallsL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0648 Transaction test with reserving disk space "));
	TransactionTestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0649 Two DBMS sessions test "));
    TwoSessTestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0650 Two DBMS sessions test-2 "));
    TwoSessTest2L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0651 DBMS OOD - OOM test 1 "));
    OOMTest1();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0652 DBMS OOD - OOM test 2 "));
    OOMTest2();

	TheTest.Next(_L("DEF057265  Panics when uninstalling a java midlet while it is running"));
	DEF057265();

	//Add tests here!
	}

TInt E32Main()
	{
	TheTest.Title();

	__UHEAP_MARK;

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	TEST(trapCleanup != NULL);

	DeleteLargeDataFiles();

	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);
	SetupTestDirectory();

	TRAP(err, RunTestsL());
	TheDb.Close();
	TheDbSession.Close();
	TheFs.Close();
	TEST2(err, KErrNone);

	DeleteDataFiles();//delete the data files used by this test

	TheTest.End();
	TheTest.Close();

	delete trapCleanup;

	__UHEAP_MARKEND;

	return 0;
	}


