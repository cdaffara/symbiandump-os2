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
#include <e32test.h>
#include <e32math.h>
#include <bautils.h>
#include <sqldb.h>
#include <f32file64.h>
#include <hal.h>

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqldb64 test"));
RFs TheFs;
RSqlDatabase TheDb;
RSqlStatement TheStmt;

_LIT(KTestDbName1, "\\test\\t_sqldb64_1.db");

const TInt64 K1Mb = 1024LL * 1024LL;
const TInt64 K1Gb = 1024LL * K1Mb;
const TInt64 K4Gb = 4LL * K1Gb;

TInt64 TheLastInsertedRowid = -1LL;

struct TTestDriveInfo
	{
	TInt	iSizeMb;	
	TBool	iWritable;	
	};
	
TTestDriveInfo TheDriveInfo[KMaxDrives];
TInt   		   TheBiggestDriveNo = -1;
TFileName 	   TheDbName;

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	TheStmt.Close();
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbName);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
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

void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);
	}

void SqlTimerPrint(const TDesC& aText, TUint32 aStartTicks, TUint32 aEndTicks)
	{
	static TInt freq = 0;
	if(freq == 0)
		{
		TEST2(HAL::Get(HAL::EFastCounterFrequency, freq), KErrNone);
		}
	TInt64 diffTicks = (TInt64)aEndTicks - (TInt64)aStartTicks;
	if(diffTicks < 0)
		{
		diffTicks = KMaxTUint32 + diffTicks + 1;
		}
	const TInt KMicroSecIn1Sec = 1000000;
	TInt32 us = (diffTicks * KMicroSecIn1Sec) / freq;
	TheTest.Printf(_L("#### %S. Execution time: %d us\r\n"), &aText, us);
	}

TUint32 SqlTimerTicks()
	{
	return User::FastCounter();
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			PDS-SQL-CT-4129
@SYMTestCaseDesc		Creation of a database bigger than 4Gb (KMaxTUint).
						The test creates a test database with a table and inserts records into the table
						until the database size gets bigger than 4Gb (KMaxTUint). The purpose of the test is to verify
						that it is possible to create and manipulate 64-bit databases.
@SYMTestActions			Creation of a database bigger than 4Gb (KMaxTUint).
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12104
                        REQ12105
*/
void CreateBigDbTest(const TDesC& aDbName, TInt64 aDbSize)
	{
	__ASSERT_ALWAYS(aDbSize > 0LL, User::Invariant());
	(void)RSqlDatabase::Delete(aDbName);
	_LIT8(KConfig, "encoding=\"UTF-8\"");
	TInt err = TheDb.Create(aDbName, &KConfig);
	TEST2(err, KErrNone);
	//
	err = TheDb.Exec(_L8("CREATE TABLE A(Id INTEGER PRIMARY KEY AUTOINCREMENT, Data BLOB)"));
	TEST2(err, 1);
	TInt64 fsize = 0;
	TheTest.Printf(_L("==File size:"));
	while(fsize < aDbSize)
		{
		const TInt KRecCnt = 1000;
		//Insert KRecCnt records in a transaction
		err = TheDb.Exec(_L8("BEGIN"));	
		if(err < 0)
			{
			TheTest.Printf(_L("==='BEGIN' has failed with err %d\r\n"), err);	
			}
		TEST(err >= 0);
		err = TheStmt.Prepare(TheDb, _L8("INSERT INTO A(Data) VALUES(zeroblob(32768))"));//32Kb big blob
		TEST2(err, KErrNone);
		for(TInt i=0;i<KRecCnt;++i)
			{
			err = TheStmt.Exec();
			TEST2(err, 1);
			err = TheStmt.Reset();
			TEST2(err, KErrNone);
			}
		TheStmt.Close();
		err = TheDb.Exec(_L8("COMMIT"));	
		if(err < 0)
			{
			TheTest.Printf(_L("==='COMMIT' has failed with err %d\r\n"), err);	
			}
		TEST(err >= 0);
		TheLastInsertedRowid = TheDb.LastInsertedRowId();
		TEST(TheLastInsertedRowid > 0LL);
		//Check and print the file size
		TheDb.Close();
		RFile64 file;
		err = file.Open(TheFs, aDbName, EFileRead | EFileWrite);
		TEST2(err, KErrNone);
		err = file.Size(fsize);
		TEST2(err, KErrNone);
		file.Close();
		TheTest.Printf(_L(" %ldMb"), fsize / K1Mb);
		err = TheDb.Open(aDbName);
		TEST2(err, KErrNone);
		}
	TheTest.Printf(_L("\r\n"));
	//
	TheDb.Close();
	}
	
/**
@SYMTestCaseID			PDS-SQL-CT-4130
@SYMTestCaseDesc		SQL operations on a 64-bit database.
						The test uses the database created in test case PDS-SQL-UT-4129.
						Simple INSERT, UPDATE, DELETE and SELECT statements are executed on the database.
						The data in the test SQL statements is such that the manipulated records are beyond the 4Gb
						file offset. Some other of the test SQL statements will perform sequential scan of the whole
						database from offset 0 to the end of the database file.
						The purpose of the test is to verify that there are no problem if the database offset is 64-bit.
@SYMTestActions			SQL operations on a 64-bit database.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12104
                        REQ12105
*/
void SimpleDbOperationsTestL(const TDesC& aDbName)
	{
	__ASSERT_ALWAYS(TheLastInsertedRowid > 0LL, User::Invariant());
	TInt err = TheDb.Open(aDbName);
	TEST2(err, KErrNone);
	//SELECT-1
	TUint32 start = SqlTimerTicks();
	err = TheStmt.Prepare(TheDb, _L8("SELECT Id FROM A WHERE ROWID = :Prm"));
	TEST2(err, KErrNone);
	err = TheStmt.BindInt64(0, TheLastInsertedRowid - 1LL);
	TEST2(err, KErrNone);
	err = TheStmt.Next();
	TEST2(err, KSqlAtRow);
	TInt64 id = TheStmt.ColumnInt64(0);
	TheTest.Printf(_L("==Id=%ld\r\n"), id);
	TheStmt.Close();
	TUint32 end = SqlTimerTicks();
	SqlTimerPrint(_L("SELECT-1"), start, end);
	//INSERT
	start = SqlTimerTicks();
	err = TheDb.Exec(_L("INSERT INTO A(Data) VALUES('123456')"));
	TEST2(err, 1);
	end = SqlTimerTicks();
	SqlTimerPrint(_L("INSERT"), start, end);
	//UPDATE
	start = SqlTimerTicks();
	TBuf<100> sql;
	sql.Format(_L("UPDATE A SET Data='56789' WHERE Id=%ld"), id);
	err = TheDb.Exec(sql);
	TEST2(err, 1);
	end = SqlTimerTicks();
	SqlTimerPrint(_L("UPDATE"), start, end);
	//SELECT-2
	start = SqlTimerTicks();
	TSqlScalarFullSelectQuery scalarQuery(TheDb);
	sql.Format(_L("SELECT Data FROM A WHERE ID = %ld"), id);
	TBuf<32> buf;
	err = scalarQuery.SelectTextL(sql, buf);
	TEST2(err, KErrNone);
	err = buf.Compare(_L("56789"));
	TEST2(err, 0);
	end = SqlTimerTicks();
	SqlTimerPrint(_L("SELECT-2"), start, end);
	//SELECT-3
	start = SqlTimerTicks();
	sql.Format(_L("SELECT COUNT(*) FROM A"));
	TInt recCnt = scalarQuery.SelectIntL(sql);
	TheTest.Printf(_L("==Records count: %d\r\n"), recCnt);
	end = SqlTimerTicks();
	SqlTimerPrint(_L("SELECT-3"), start, end);
	TEST(recCnt > 0);
	//SELECT-4
	start = SqlTimerTicks();
	sql.Format(_L("SELECT MAX(ROWID) FROM A"));
	TInt rowid = scalarQuery.SelectIntL(sql);
	TheTest.Printf(_L("==MAX(ROWID): %d\r\n"), rowid);
	end = SqlTimerTicks();
	SqlTimerPrint(_L("SELECT-4"), start, end);
	TEST(rowid > 0);
	//DELETE
	start = SqlTimerTicks();
	sql.Format(_L("DELETE FROM A WHERE ID = %ld"), id);
	err = TheDb.Exec(sql);
	TEST2(err, 1);
	end = SqlTimerTicks();
	SqlTimerPrint(_L("DELETE"), start, end);
	//
	TheDb.Close();
	}

/**
@SYMTestCaseID			PDS-SQL-CT-4145
@SYMTestCaseDesc		RSqlDatabase::Size() on a 64-bit database.
						The test uses the database created in test case PDS-SQL-UT-4129, opens the database
						and calls the Size() methods. The first Size() call should fail with KErrTooBig error,
						because the database size is over 2Gb and cannot fit in the 32-bit integer result of the call.
						The second call of the overloaded Size() method should correctly report the database size.
@SYMTestActions			RSqlDatabase::Size() on a 64-bit database.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12105
*/
void SizeTest(const TDesC& aDbName)
	{
	__ASSERT_ALWAYS(TheLastInsertedRowid > 0LL, User::Invariant());
	TInt err = TheDb.Open(aDbName);
	TEST2(err, KErrNone);
	//Size-1
	TInt size = TheDb.Size();
	TEST2(size, KErrTooBig);
	//Size-2
	RSqlDatabase::TSize size2;
	err = TheDb.Size(size2);
	TEST2(err, KErrNone);
	TEST(size2.iSize > K4Gb);
	//
	TheDb.Close();
	}

/**
@SYMTestCaseID			PDS-SQL-CT-4146
@SYMTestCaseDesc		Background compaction on a 64-bit database.
						The test uses the database created in test case PDS-SQL-UT-4129 and opens the database.
						Iteration 1:
						The test executes a DELETE sql statement that deletes couple of records. The freed disk space
						is big enough to kick-off the background compaction. The test waits couple of seconds and then
						checks the database size and free space to verify that the background compaction really compacted
						the database.
						Iteration 2:
						Iteration 2 is the same as iteration 1, but the freed database space is such that the following 
						background compaction will shrink the database size to be less than 4Gb.
						After iteration 2 the test performs an INSERT transaction and increases the database size to be
						bigger than 4Gb.
@SYMTestActions			Background compaction on a 64-bit database.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12105
*/
void BackgroundCompactionTest(const TDesC& aDbName)
	{
	__ASSERT_ALWAYS(TheLastInsertedRowid > 0LL, User::Invariant());
	TInt err = TheDb.Open(aDbName);
	TEST2(err, KErrNone);
	const TInt64 KDelRecCnt[2] = {10LL, 2400LL};
	for(TInt i=0;i<2;++i)
		{
		TheTest.Printf(_L("=========== Iteration %d ===========\r\n"), i + 1);
		//Size-1
		RSqlDatabase::TSize size;
		err = TheDb.Size(size);
		TEST2(err, KErrNone);
		TEST(size.iSize > K4Gb);
		TheTest.Printf(_L(" ==Before DELETE, database size=%ldKb\r\n"), size.iSize / 1024LL);
		//Delete records
		TBuf<100> sql;
		sql.Format(_L("DELETE FROM A WHERE ROWID > %ld AND ROWID < %ld"), TheLastInsertedRowid - KDelRecCnt[i], TheLastInsertedRowid);
		err = TheDb.Exec(sql);
		TEST(err > 0);
		//Size-2
		err = TheDb.Size(size);
		TEST2(err, KErrNone);
		TEST(size.iSize > K4Gb);
		TEST(size.iFree > (50 * 1024));
		TheTest.Printf(_L(" ==After DELETE, database size=%ldKb, free space=%ldKb\r\n"), size.iSize / 1024LL, size.iFree / 1024LL);
		//Wait some time (to allow the background compaction to run)
		const TInt KOneSecond = 1000000;
		const TInt KMaxWaitTime = 300 * KOneSecond;//300 sec == 5 min
		TInt waitTime = 0;
		while(waitTime < KMaxWaitTime)
			{
			const TInt KWaitStep = 5 * KOneSecond;
			User::After(KWaitStep);
			//Check the size
			err = TheDb.Size(size);
			TEST2(err, KErrNone);
			if(size.iFree == 0)
				{
				break;	
				}
			waitTime += KWaitStep;
			TheTest.Printf(_L(" ==After %3d sec, database size=%ldKb, free space=%ldKb\r\n"), waitTime / KOneSecond, size.iSize / 1024LL, size.iFree / 1024LL);
			}
		if(i == 0)
			{
			TEST(size.iSize > K4Gb);
			}
		else
			{
			TEST(size.iSize < K4Gb);
			}
		TEST2(size.iFree, 0);
		//Records count
		sql.Format(_L("SELECT COUNT(*) FROM A"));
		TSqlScalarFullSelectQuery q(TheDb);
		TInt recCnt = -1;
		TRAP(err, recCnt = q.SelectIntL(sql));
		TEST2(err, KErrNone);
		TheTest.Printf(_L(" ==Records count: %d\r\n"), recCnt);
		TEST(recCnt > 0);
		}
	TheTest.Printf(_L("==Increase the database size above 4Gb\r\n"));
	//Insert KRecCnt records in a transaction
	const TInt KRecCnt = 2500;
	err = TheDb.Exec(_L8("BEGIN"));	
	TEST(err >= 0);
	err = TheStmt.Prepare(TheDb, _L8("INSERT INTO A(Data) VALUES(zeroblob(32768))"));//32Kb big blob
	TEST2(err, KErrNone);
	for(TInt i=0;i<KRecCnt;++i)
		{
		err = TheStmt.Exec();
		TEST2(err, 1);
		err = TheStmt.Reset();
		TEST2(err, KErrNone);
		}
	TheStmt.Close();
	err = TheDb.Exec(_L8("COMMIT"));	
	TEST(err >= 0);
	//Size
	RSqlDatabase::TSize size2;
	err = TheDb.Size(size2);
	TEST2(err, KErrNone);
	TEST(size2.iSize > K4Gb);
	TheTest.Printf(_L(" ==Database size=%ldKb\r\n"), size2.iSize / 1024LL);
	//		
	TheDb.Close();
	}

/**
@SYMTestCaseID			PDS-SQL-CT-4131
@SYMTestCaseDesc		Deleting database bigger than 4Gb (KMaxTUint).
						The test deletes the database created in test case PDS-SQL-UT-4129.
@SYMTestActions			Deleting database bigger than 4Gb (KMaxTUint).
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12104
                        REQ12105
*/
void DeleteBigDbTest()
	{
	TInt err = RSqlDatabase::Delete(TheDbName);
	TEST2(err, KErrNone);
	}

void CollectDriveInfo()
	{
	TheTest.Printf(_L("==================\r\n"));
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
	
	Mem::FillZ(TheDriveInfo, sizeof(TheDriveInfo));
	TheBiggestDriveNo = 0;
	
	for(TInt drive=EDriveA;drive<=EDriveZ;++drive)
		{
		TDriveInfo driveInfo;
		TInt err = TheFs.Drive(driveInfo, drive);
		if(err == KErrNone)
			{
			TVolumeInfo vinfo;
			err = TheFs.Volume(vinfo, drive);
			if(err == KErrNone)
				{
				TVolumeIOParamInfo vparam;
				err = TheFs.VolumeIOParam(drive, vparam);
				TEST2(err, KErrNone);
				TBuf8<128> vinfoex8;
				err = TheFs.QueryVolumeInfoExt(drive, EFileSystemSubType, vinfoex8);
				TEST2(err, KErrNone);
				TPtrC vinfoex((const TUint16*)(vinfoex8.Ptr() + 8), vinfoex8[0]);
				TPtrC KMediaTypeNames[] = {KType1(), KType2(), KType3(), KType4(), KType5(), KType6(), KType7(), KType8(), KType9(), KType10(), KType11()};
				TInt sizeMb = vinfo.iSize / K1Mb;
				TheTest.Printf(_L("Drive: %C:, Type: %16.16S, File System: %8.8S, Size: %d Mb.\r\n"), 'A' + drive, &KMediaTypeNames[driveInfo.iType], &vinfoex, sizeMb);
				TheTest.Printf(_L("       Block size=%d, Cluster size=%d, Read buffer size=%d.\r\n"), vparam.iBlockSize, vparam.iClusterSize, vparam.iRecReadBufSize);
				TheDriveInfo[drive].iSizeMb = sizeMb;
				if(driveInfo.iType == EMediaRam || driveInfo.iType == EMediaHardDisk || driveInfo.iType == EMediaFlash || driveInfo.iType == EMediaNANDFlash)
				  	{
					TheDriveInfo[drive].iWritable = ETrue;
					if(sizeMb > TheDriveInfo[TheBiggestDriveNo].iSizeMb)
						{
						TheBiggestDriveNo = drive;
						}
					}
				}
			else
				{
				TheTest.Printf(_L("Drive %C. RFs::Volume() has failed with err=%d.\r\n"), 'A' + drive, err);	
				}
			}
		else
			{
			TheTest.Printf(_L("Drive %C. RFs::Drive() has failed with err=%d.\r\n"), 'A' + drive, err);	
			}
		}
		
	TheTest.Printf(_L("The biggest R/W drive is: %C, Size: %d Mb\r\n"), 'A' + TheBiggestDriveNo, TheDriveInfo[TheBiggestDriveNo].iSizeMb);
	TDriveUnit drvUnit(TheBiggestDriveNo);
	TDriveName drvName = drvUnit.Name();
	TParse parse;
	parse.Set(KTestDbName1, &drvName, NULL);
	TheDbName.Copy(parse.FullName());

	TRAPD(err, BaflUtils::EnsurePathExistsL(TheFs, TheDbName));
	TEST(err == KErrNone || err == KErrAlreadyExists);

	TheTest.Printf(_L("==================\r\n"));
	}

void DoTestsL()
	{
	TheTest.Start(_L("Collect drive information"));
	CollectDriveInfo();
	
	TInt64 maxDrvSize = TheDriveInfo[TheBiggestDriveNo].iSizeMb * K1Mb;
	if(maxDrvSize <= K4Gb)
		{
		TheTest.Printf(_L("There is no drive bigger than 4Gb. The tests won't be executed.\r\n"));
		return;	
		}
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4129 Create database, bigger than 4Gb"));
	CreateBigDbTest(TheDbName, K4Gb + 64 * K1Mb);
	
	TheTest.Next (_L(" @SYMTestCaseID:PDS-SQL-CT-4130 64-bit database - simple operations test"));
	SimpleDbOperationsTestL(TheDbName);
	
	TheTest.Next (_L(" @SYMTestCaseID:PDS-SQL-CT-4145 64-bit database - Size() test"));
	SizeTest(TheDbName);
	
	TheTest.Next (_L(" @SYMTestCaseID:PDS-SQL-CT-4146 64-bit database - background compaction test"));
	BackgroundCompactionTest(TheDbName);
	
	TheTest.Next (_L(" @SYMTestCaseID:PDS-SQL-CT-4131 Delete 64-bit database test"));
	DeleteBigDbTest();
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);

	__UHEAP_MARK;

	CreateTestEnv();
	DeleteTestFiles();
	TRAPD(err, DoTestsL());
	DeleteTestFiles();
	TheFs.Close();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
