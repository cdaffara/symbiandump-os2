// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

///////////////////////////////////////////////////////////////////////////////////////

static RFs TheFs;
RTest TheTest(_L("t_sqlood test"));

#if  defined __WINSCW__ || defined __WINS__

	//The C: drive may be too big and may be used concurently by other applications. 
	//The T: drive is more suitable for the test if running on the emulator
	const TInt KTestDrive = EDriveT;
	_LIT(KTestDir, "t:\\test\\");
	_LIT(KTestDatabase, "t:\\test\\t_sql_ood.db");
	
#elif defined __X86GCC__

	const TInt KTestDrive = EDriveG;
	_LIT(KTestDir, "g:\\test\\");
	_LIT(KTestDatabase, "g:\\test\\t_sql_ood.db");
	
#else

	const TInt KTestDrive = EDriveE;
	_LIT(KTestDir, "e:\\test\\");
	_LIT(KTestDatabase, "e:\\test\\t_sql_ood.db");
	
#endif

//One or more files with KLargeFileName name and ".<n>" extension, where n is 
//000, 001, 002, 003...
//will be created and they will occupy almost all available disk space.
//The idea is to perform after that one "delete"
//transaction, which must to fail, because there won't be enough available disk space to complete the transaction.
#if  defined __WINSCW__ || defined __WINS__

	_LIT(KLargeFileName, "t:\\test\\DeleteMe");

#elif defined __X86GCC__

	_LIT(KLargeFileName, "g:\\test\\DeleteMe");

#else

	_LIT(KLargeFileName, "e:\\test\\DeleteMe");

#endif

_LIT8(KDatabasePageSizeConfig, "page_size=1024");

const TInt KMaxTestRecordsCount = 350;
TInt TestRecordsCount = 0;

///////////////////////////////////////////////////////////////////////////////////////

//Assemblesd a file name from "aFileName" and "aFileNumber" parameters and places the resulting string in "aResultPath".
void AssembleLargeFileName(const TDesC& aFileName, TInt aFileNumber, TDes& aResultPath)
	{
	_LIT(KFormatStr, "%S.%03d");
	aResultPath.Format(KFormatStr, &aFileName, aFileNumber);
	}

//Deletes all created large data files.
void DeleteLargeDataFiles()
	{
	TInt err = KErrNone;
	TInt i = -1;
	while(err == KErrNone)
		{
		TBuf<KMaxFileName> filePath;
		::AssembleLargeFileName(KLargeFileName, ++i, filePath);
		err = TheFs.Delete(filePath);
		}
	}

//Deletes all created test files.
void DeleteTestFiles()
	{
	DeleteLargeDataFiles();
	(void)RSqlDatabase::Delete(KTestDatabase);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
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
	if(err != KErrNone)
	    {
	    RDebug::Print(_L("*** CreateTestEnv(), RFs::MkDir(), err=%d\r\n"), err);
	    }
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

//Creates one or more large files with the total size near to the size of the available disk space.
//The idea is to cause an "out of disk space" condition.
void FillLargeDataFile(RFile& aFile, TInt aSize)
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

//Gets and returns the available disk space of the tested drive.
TInt64 FreeDiskSpace()
	{
	TVolumeInfo volInfoBefore;
	TInt err = TheFs.Volume(volInfoBefore, KTestDrive);
	TEST2(err, KErrNone);
	return volInfoBefore.iFree;
	}

//Creates a large data file with aSize size (in bytes).
void DoCreateLargeFile(const TDesC& aPath, TInt aSize)
	{
	RFile file;
	TInt err = file.Replace(TheFs, aPath, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	FillLargeDataFile(file, aSize);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	}

//Creates enough number of large data files to fill the available disk space.
void CreateLargeFile()
	{
	TInt fileNo = 0;
	const TInt KLargeFileSize = 1000000000;
	TInt64 diskSpace = ::FreeDiskSpace();
	RDebug::Print(_L("CreateLargeFile: free space before = %ld\r\n"), diskSpace);
	TBuf<KMaxFileName> filePath;
	while(diskSpace > KLargeFileSize)
		{
		AssembleLargeFileName(KLargeFileName, fileNo++, filePath);
		DoCreateLargeFile(filePath, KLargeFileSize);
		diskSpace = ::FreeDiskSpace();
		RDebug::Print(_L("----CreateLargeFile, step %d, free space = %ld\r\n"), fileNo, diskSpace);
		}
	//Reserve almost all disk space, except a small amount - 200 bytes.
	if(diskSpace > 0)
		{
		::AssembleLargeFileName(KLargeFileName, fileNo++, filePath);
		const TInt64 KSpaceLeft = 200;
		TInt64 lastFileSize = diskSpace - KSpaceLeft;
        TInt lastFileSize32 = I64LOW(lastFileSize);
		RDebug::Print(_L("----file size32 = %d\r\n"), lastFileSize32);
		::DoCreateLargeFile(filePath, lastFileSize32);
		RDebug::Print(_L("----CreateLargeFile, last step (%d), file size = %ld\r\n"), fileNo, lastFileSize);
		}
	diskSpace = ::FreeDiskSpace();
	RDebug::Print(_L("CreateLargeFile: free space after = %ld\r\n"), diskSpace);
	}


// Number of bytes in the default journal header size.
const TInt KJournalHeaderSize = 0x200;

// Number of bytes added to each database page in the journal.
const TInt KJournalPageOverhead = 8; 

// The default amount of reserved space provided by the ReserveDriveSpace API
const TInt KReserveDriveSpaceAmount = 64*1024;

//Creates and fills with some records a test database
void CreateAndFillTestDatabase(RSqlDatabase& aDb)
	{
	TInt err = aDb.Create(KTestDatabase, &KDatabasePageSizeConfig);
	TEST2(err, KErrNone);
	err = aDb.Exec(_L("CREATE TABLE A(Id INTEGER, Data TEXT)"));
	TEST(err >= 0);

	//
	// Find the page size of the database on this media
	//
	TBuf<200> sql;
	sql.Copy(_L("PRAGMA page_size"));
	TSqlScalarFullSelectQuery q(aDb);
	TInt pageSize = 0;
	TRAP(err, pageSize = q.SelectIntL(sql););
	//RDebug::Print(_L("Error %d Page Size %d"),err,pageSize);
	TEST2(err, KErrNone);
	TEST(pageSize > 0);
	//RDebug::Print(_L("Page Size %d"),pageSize);
	
	//
	// Find the sector size of this media
	//
	TDriveInfo driveInfo;
	err = TheFs.Drive(driveInfo, KTestDrive);
	TEST2(err, KErrNone);
	TVolumeIOParamInfo volumeInfo;
	err = TheFs.VolumeIOParam(KTestDrive, volumeInfo);
	TEST2(err, KErrNone);
	TInt sectorSize = volumeInfo.iBlockSize;
	//RDebug::Print(_L("Sector Size %d"),sectorSize);	

	TInt journalHeaderSize = Max(sectorSize, KJournalHeaderSize);
	//RDebug::Print(_L("Journal Header Size %d"),journalHeaderSize);

	//
	// Keep adding to database until it is a size such that all the data can still be deleted within the reserved disk space size. 
	// Do this piece-wise not in a transaction.
	//
	TInt i;
	for(i=0;i<KMaxTestRecordsCount;++i)
		{
		sql.Format(_L("INSERT INTO A(Id, Data) VALUES(%d, 'A0123456789B0123456789C0123456789D0123456789E0123456789F0123456789G0123456789H0123456789')"), i + 1);
		err = aDb.Exec(sql);
		TEST2(err, 1);

		TInt size = aDb.Size();
		TInt numberOfPages = size/pageSize;
		TInt predictedJournalSize = journalHeaderSize + numberOfPages * (pageSize + KJournalPageOverhead);
		//RDebug::Print(_L("Size %d, Pages %d, predictedJournalSize %d"),size, numberOfPages, predictedJournalSize);
		
		// Will another page take us over the limit ?
		if ((predictedJournalSize + (pageSize + KJournalPageOverhead)) >= (KReserveDriveSpaceAmount))
			{
			break;
			}
		}
	TestRecordsCount = i + 1;
	
	//RDebug::Print(_L("TestRecordsCount %d"),TestRecordsCount);
	
	}

//Tries to delete test database records
TInt DeleteTestRecords(RSqlDatabase& aDb)
	{
	TInt err = aDb.Exec(_L("BEGIN TRANSACTION"));
	TEST(err >= 0);
	for(TInt i=0;i<TestRecordsCount;++i)
		{
		TBuf<100> sql;
		sql.Format(_L("DELETE FROM A WHERE Id = %d"), i + 1);
		err = aDb.Exec(sql);// May fail with KErrDiskFull
		if(err < 0)
			{
			(void)aDb.Exec(_L("ROLLBACK TRANSACTION"));
			return err;
			}
		}
	err = aDb.Exec(_L("COMMIT TRANSACTION"));// May fail with KErrDiskFull
	return err;
	}

///////////////////////////////////////////////////////////////////////////////////////

//The function simply calls RSqlDatabase::ReserveDriveSpace(), RSqlDatabase::GetReserveAccess(),
//RSqlDatabase::ReleaseReserveAccess() methods and checks the return values.
//It might be usefull for debugging in case if something gets wrong.
void SimpleCallsTest()
	{
	RSqlDatabase db, db2;
	TInt err = db.Create(KTestDatabase, &KDatabasePageSizeConfig);
	TEST2(err, KErrNone);

	err = db2.Open(KTestDatabase);
	TEST2(err, KErrNone);
	
	//An attempt to get an access to the reserved space (which is not reserved yet).
	err = db.GetReserveAccess();
	TEST2(err, KErrNotFound);

	//Reserve disk space
	err = db.ReserveDriveSpace(0);
	TEST2(err, KErrNone);

	//An attempt to re-reserve it
   	err = db.ReserveDriveSpace(0);
	TEST2(err, KErrAlreadyExists);

	//Get an access to the reserved disk space
	err = db.GetReserveAccess();
	TEST2(err, KErrNone);

	//Reserve disk space from the second connection
	err = db2.ReserveDriveSpace(0);
	TEST2(err, KErrNone);

	//An attempt to get an access to the reserved space twice.
	err = db.GetReserveAccess();
	TEST2(err, KErrInUse);

	//An attempt to get an access to the reserved space from the second connection.
	err = db2.GetReserveAccess();
	TEST2(err, KErrNone);

	db.ReleaseReserveAccess();

	//An attempt to release the reserved space twice.
	db.ReleaseReserveAccess();

	//Free the reserved disk space
	db.FreeReservedSpace();

	//Free the reserved disk space twice.
	db.FreeReservedSpace();

	//Free the reserved disk space from the second connection.
	db2.FreeReservedSpace();

	db2.Close();
	db.Close();
	(void)RSqlDatabase::Delete(KTestDatabase);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1649
@SYMTestCaseDesc		SQL database "out of disk space" tests.
						The test creates and fills with some records a test database and then reserves a disk space.
						The second step: the test fills almost all available disk space creting large data files.
						The third step: the test attempts to delete all records from the test database and fails with
						KErrDiskFull error.
						The fourth step: the test gets an access to the reserved disk space and attempts to delete
						records again. This time the test should not fail.
@SYMTestPriority		High
@SYMTestActions			SQL database "out of disk space" tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/
void DeleteTransactionTest()
	{
	TVolumeIOParamInfo volIoPrm;
	TInt err = TheFs.VolumeIOParam(KTestDrive, volIoPrm);
    TEST2(err, KErrNone);
    RDebug::Print(_L("--Drive %d. BlockSize=%d, ClusterSize=%d, RecReadBufSize=%d, RecWriteBufSize=%d\r\n"), KTestDrive, volIoPrm.iBlockSize, volIoPrm.iClusterSize, volIoPrm.iRecReadBufSize, volIoPrm.iRecWriteBufSize);
	/////////////////////////////////////////////////////////
    RDebug::Print(_L("--Create and fill database \"%S\".\r\n"), &KTestDatabase);
	RSqlDatabase db;
	CreateAndFillTestDatabase(db);
	db.Close();//When the database gets closed, the persisted journal file will be deleted.
    RDebug::Print(_L("--Close and reopen database \"%S\" (in order to get the persisted journal file deleted).\r\n"), &KTestDatabase);
    err = db.Open(KTestDatabase);
    TEST2(err, KErrNone);
    RDebug::Print(_L("--Reserve disk space for database \"%S\".\r\n"), &KTestDatabase);
    err = db.ReserveDriveSpace(0);
	TEST2(err, KErrNone);
    RDebug::Print(_L("--Simulate an \"out of disk space\" situation with creating a very large data file, which occupies almost the all the available disk space.\r\n"));
	CreateLargeFile();
	RDebug::Print(_L("--Attempt to delete test data records. The transaction must fail, because of \"out of disk space\".\r\n"));
	err = DeleteTestRecords(db);
	TEST2(err, KErrDiskFull);
    RDebug::Print(_L("--Get an access to the reserved disk space.\r\n"));
	err = db.GetReserveAccess();
	TEST2(err, KErrNone);
    TInt64 diskSpace = ::FreeDiskSpace();
    RDebug::Print(_L("After GetReserveAccess(), free disk space = %ld. Try again \"Delete records\" transaction. The transaction must not fail.\r\n"), diskSpace);
	err = DeleteTestRecords(db);
	RDebug::Print(_L("--DeleteTestRecords() returned %d error.\r\n"), err);
	TEST(err >= 0);
	//Releases the access to the reserved disk space
	db.ReleaseReserveAccess();
	//Frees the reserved disk space
	db.FreeReservedSpace();
    //Free the resources, used in the test
	DeleteLargeDataFiles();
	//Verify that the records have been deleted
	RSqlStatement stmt;
	err = stmt.Prepare(db, _L("SELECT COUNT(*) FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TInt recCount = stmt.ColumnInt(0);
	TEST2(recCount, 0);
	stmt.Close();
	db.Close();
	(void)RSqlDatabase::Delete(KTestDatabase);
	}

//OOD API tests with more than one connection to the same SQL database.
//The test calls ReserveDriveSpace/GetReserveAccess/ReleaseReserveAccess in a different
//combinations on four RSqlDatabase objects, connected to the same database .
//The test should not fail or panic.
void MultiDbTest()
    {
    RSqlDatabase db1;
	CreateAndFillTestDatabase(db1);

    RSqlDatabase db2;
    TInt err = db2.Open(KTestDatabase);
    TEST2(err, KErrNone);

    //Play with "ReserveDriveSpace" on both sessions
    err = db1.ReserveDriveSpace(0);
    TEST2(err, KErrNone);
    err = db2.ReserveDriveSpace(0);
    TEST2(err, KErrNone);
    db2.FreeReservedSpace();
    err = db2.ReserveDriveSpace(0);
    TEST2(err, KErrNone);

    //Get an access to the reserved space through db2
	err = db2.GetReserveAccess();
    TEST2(err, KErrNone);
    //Free/re-reserve disk space for db1.
    db1.FreeReservedSpace();
    err = db1.ReserveDriveSpace(0);
    TEST2(err, KErrNone);

    RSqlDatabase db4;
    err = db4.Open(KTestDatabase);
    TEST2(err, KErrNone);

    //Try to reserve space for db4.
    err = db4.ReserveDriveSpace(0);
    TEST2(err, KErrNone);

    RSqlDatabase db3;
    err = db3.Open(KTestDatabase);
    TEST2(err, KErrNone);

    //Try to reserve space for session db3.
    err = db3.ReserveDriveSpace(0);
    TEST2(err, KErrNone);

    //Release and free db2 access to the reserved space.
    db2.ReleaseReserveAccess();
    db2.FreeReservedSpace();

    db3.FreeReservedSpace();
    db3.Close();

    db4.FreeReservedSpace();
    db4.Close();

    //Get an access to the reserved space through db2.
    //But it was freed, so the call will fail.
	err = db2.GetReserveAccess();
    TEST2(err, KErrNotFound);

    //Free/re-reserve disk space for db1.
    db1.FreeReservedSpace();
    err = db1.ReserveDriveSpace(0);
    TEST2(err, KErrNone);

    //Get/release the access to the reserved space for db1.
	err = db1.GetReserveAccess();
    TEST2(err, KErrNone);
    db1.ReleaseReserveAccess();

    //Get an access to the reserved space for db2.
    //The call will fail because there is no reserved disk space for db2.
	err = db2.GetReserveAccess();
    TEST2(err, KErrNotFound);

    //Free the reserved space - db1
    db1.FreeReservedSpace();

	db2.Close();
	db1.Close();

	(void)RSqlDatabase::Delete(KTestDatabase);
    }

void DoTests()
	{
	TheTest.Start(_L(" \"Simple calls\" OOD test "));
	SimpleCallsTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1649 \"Delete transaction\" OOD test "));
	DeleteTransactionTest();

	TheTest.Next(_L(" Multi db OOD test "));
	MultiDbTest();
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;

	CreateTestEnv();
	DeleteTestFiles();
	DoTests();
	DeleteTestFiles();
	TheFs.Close();

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
