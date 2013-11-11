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
#include <bautils.h>
#include <f32file64.h>
#include <e32math.h>
#include <hal.h>
#include <sqlite3.h>
#include <string.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlitedb64 test"));

_LIT(KTestDbName,	"\\test\\t_sqlitedb64.db");

RFs TheFs;

sqlite3* TheDb = 0;
sqlite3_stmt* TheStmt = 0;

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
char		   TheDbName8[KMaxFileName];

///////////////////////////////////////////////////////////////////////////////////////

static void DeleteTestFiles()
	{
	if(TheStmt)
		{
		sqlite3_finalize(TheStmt);	
		TheStmt = 0;
		}
	if(TheDb)
		{
		(void)sqlite3_close(TheDb);
		TheDb = 0;
		}
	(void)TheFs.Delete(TheDbName);
	}

///////////////////////////////////////////////////////////////////////////////////////
static void PrintSqliteErrMsg()
	{
	if(TheDb)
		{
		const char* msg = sqlite3_errmsg(TheDb);
		TBuf<200> buf;
		buf.Copy(TPtrC8((const TUint8*)msg));
		TheTest.Printf(_L("*** SQLite error message: \"%S\"\r\n"), &buf);
		}
	}

//Test macros and functions
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		PrintSqliteErrMsg();
		TheTest(EFalse, aLine);
		}
	}
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		PrintSqliteErrMsg();
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

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
	parse.Set(KTestDbName, &drvName, NULL);
	TheDbName.Copy(parse.FullName());
	TPtr8 p((TUint8*)TheDbName8, 0, KMaxFileName);
	p.Copy(TheDbName);
	p.Append(TChar(0));

	TRAPD(err, BaflUtils::EnsurePathExistsL(TheFs, TheDbName));
	TEST(err == KErrNone || err == KErrAlreadyExists);

	TheTest.Printf(_L("==================\r\n"));
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			PDS-SQLITE3-CT-4041
@SYMTestCaseDesc		Creation of a database bigger than 4Gb (KMaxTUint).
						The test creates a test database with a table and inserts records into the table
						until the database size gets bigger than 4Gb (KMaxTUint). The purpose of the test is to verify
						that it is possible to create and manipulate 64-bit SQLite databases.
@SYMTestActions			Creation of a database bigger than 4Gb (KMaxTUint).
@SYMTestExpectedResults Test must not fail
@SYMTestPriority 	    High
@SYMREQ					REQ12107
                        REQ12108
*/
void CreateBigDbTest(TInt64 aDbSize)
	{
	__ASSERT_ALWAYS(aDbSize > 0LL, User::Invariant());
	(void)TheFs.Delete(TheDbName);

	const char* ver = sqlite3_libversion();
	TBuf<20> buf;
	buf.Copy(TPtrC8((const TUint8*)ver));
	TheTest.Printf(_L("*** SQLite library version: \"%S\"\r\n"), &buf);
	
	TInt err = sqlite3_open(TheDbName8, &TheDb);
	TEST2(err, SQLITE_OK);
	
	//
	err = sqlite3_exec(TheDb, "CREATE TABLE A(Id INTEGER PRIMARY KEY AUTOINCREMENT, Data BLOB)", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	TInt64 fsize = 0;
	TheTest.Printf(_L("==File size:"));
	while(fsize < aDbSize)
		{
		const TInt KRecCnt = 1000;
		//Insert KRecCnt records in a transaction
		err = sqlite3_exec(TheDb, "BEGIN", 0, 0, 0);	
		if(err != SQLITE_OK)
			{
			TheTest.Printf(_L("==='BEGIN' has failed with err %d\r\n"), err);
			}
		TEST2(err, SQLITE_OK);
		err = sqlite3_prepare(TheDb, "INSERT INTO A(Data) VALUES(zeroblob(32768))", -1, &TheStmt, 0);//32Kb big blob
		TEST2(err, SQLITE_OK);
		for(TInt i=0;i<KRecCnt;++i)
			{
			err = sqlite3_step(TheStmt);
			TEST2(err, SQLITE_DONE);
			err = sqlite3_reset(TheStmt);
			TEST2(err, SQLITE_OK);
			}
		err = sqlite3_finalize(TheStmt);
		TEST2(err, SQLITE_OK);
		TheStmt = 0;
		err = sqlite3_exec(TheDb, "COMMIT", 0, 0, 0);	
		if(err != SQLITE_OK)
			{
			TheTest.Printf(_L("==='COMMIT' has failed with err %d\r\n"), err);	
			}
		TEST2(err, SQLITE_OK);
		TheLastInsertedRowid = sqlite3_last_insert_rowid(TheDb);
		TEST(TheLastInsertedRowid > 0LL);
		//Check and print the file size
		sqlite3_close(TheDb);
		TheDb = 0;
		RFile64 file;
		err = file.Open(TheFs, TheDbName, EFileRead | EFileWrite);
		TEST2(err, KErrNone);
		err = file.Size(fsize);
		TEST2(err, KErrNone);
		file.Close();
		TheTest.Printf(_L(" %ldMb"), fsize / K1Mb);
		err = sqlite3_open(TheDbName8, &TheDb);
		TEST2(err, SQLITE_OK);
		}
	TheTest.Printf(_L("\r\n"));
	//
	sqlite3_close(TheDb);
	TheDb = 0;
	}

/**
@SYMTestCaseID			PDS-SQLITE3-CT-4042
@SYMTestCaseDesc		SQLite operations on a 64-bit database.
						The test uses the database created in test case PDS-SQLITE3-UT-4041.
						Simple INSERT, UPDATE, DELETE and SELECT statements are executed on the database.
						The data in the test SQL statements is such that the manipulated records are beyond the 4Gb
						file offset. Some other of the test SQL statements will perform sequential scan of the whole
						database from offset 0 to the end of the database file.
						The purpose of the test is to verify that there are no problem if the database offset is 64-bit.
@SYMTestActions			SQLite operations on a 64-bit database.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority 	    High
@SYMREQ					REQ12107
                        REQ12108
*/
void SimpleDbOperationsTest()
	{
	__ASSERT_ALWAYS(TheLastInsertedRowid > 0LL, User::Invariant());
	TInt err = sqlite3_open(TheDbName8, &TheDb);
	TEST2(err, SQLITE_OK);

	//SELECT-1
	TUint32 start = SqlTimerTicks();
	err = sqlite3_prepare(TheDb, "SELECT Id FROM A WHERE ROWID = :Prm", -1, &TheStmt, 0);
	TEST2(err, SQLITE_OK);
	err = sqlite3_bind_int64(TheStmt, 1, TheLastInsertedRowid - 1LL);
	TEST2(err, SQLITE_OK);
	err = sqlite3_step(TheStmt);
	TEST2(err, SQLITE_ROW);
	TInt64 id = sqlite3_column_int64(TheStmt, 0);
	TheTest.Printf(_L("==Id=%ld\r\n"), id);
	sqlite3_finalize(TheStmt);
	TheStmt = 0;
	TUint32 end = SqlTimerTicks();
	SqlTimerPrint(_L("SELECT-1"), start, end);

	//INSERT
	start = SqlTimerTicks();
	err = sqlite3_exec(TheDb, "INSERT INTO A(Data) VALUES('123456')", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	end = SqlTimerTicks();
	TInt cnt = sqlite3_changes(TheDb);
	TEST2(cnt, 1);
	SqlTimerPrint(_L("INSERT"), start, end);
	
	//UPDATE
	start = SqlTimerTicks();
	TBuf<100> sql;
	sql.Format(_L("UPDATE A SET Data='56789' WHERE Id=%ld"), id);
	TBuf8<100> sql8;
	sql8.Copy(sql);
	err = sqlite3_exec(TheDb, (const char*)sql8.PtrZ(), 0, 0, 0);
	TEST2(err, SQLITE_OK);
	end = SqlTimerTicks();
	cnt = sqlite3_changes(TheDb);
	TEST2(cnt, 1);
	SqlTimerPrint(_L("UPDATE"), start, end);
	
	//SELECT-2
	start = SqlTimerTicks();
	sql.Format(_L("SELECT Data FROM A WHERE ID = %ld"), id);
	sql8.Copy(sql);
	err = sqlite3_prepare(TheDb, (const char*)sql8.PtrZ(), -1, &TheStmt, 0);
	TEST2(err, SQLITE_OK);
	err = sqlite3_step(TheStmt);
	TEST2(err, SQLITE_ROW);
	const char* data = (const char*)sqlite3_column_text(TheStmt, 0);
	TEST(data != 0);
	err = strcmp(data, "56789");
	TEST2(err, 0);
	sqlite3_finalize(TheStmt);
	TheStmt = 0;
	end = SqlTimerTicks();
	SqlTimerPrint(_L("SELECT-2"), start, end);
	
	//SELECT-3
	start = SqlTimerTicks();
	err = sqlite3_prepare(TheDb, "SELECT COUNT(*) FROM A", -1, &TheStmt, 0);
	TEST2(err, SQLITE_OK);
	err = sqlite3_step(TheStmt);
	TEST2(err, SQLITE_ROW);
	TInt recCnt = sqlite3_column_int(TheStmt, 0);
	TheTest.Printf(_L("==Records count: %d\r\n"), recCnt);
	sqlite3_finalize(TheStmt);
	TheStmt = 0;
	end = SqlTimerTicks();
	SqlTimerPrint(_L("SELECT-3"), start, end);
	TEST(recCnt > 0);

	//SELECT-4
	start = SqlTimerTicks();
	err = sqlite3_prepare(TheDb, "SELECT MAX(ROWID) FROM A", -1, &TheStmt, 0);
	TEST2(err, SQLITE_OK);
	err = sqlite3_step(TheStmt);
	TEST2(err, SQLITE_ROW);
	TInt rowid = sqlite3_column_int(TheStmt, 0);
	TheTest.Printf(_L("==MAX(ROWID): %d\r\n"), recCnt);
	sqlite3_finalize(TheStmt);
	TheStmt = 0;
	end = SqlTimerTicks();
	SqlTimerPrint(_L("SELECT-4"), start, end);
	TEST(rowid > 0);
	
	//DELETE
	start = SqlTimerTicks();
	sql.Format(_L("DELETE FROM A WHERE ID = %ld"), id);
	sql8.Copy(sql);
	err = sqlite3_exec(TheDb, (const char*)sql8.PtrZ(), 0, 0, 0);
	TEST2(err, SQLITE_OK);
	end = SqlTimerTicks();
	cnt = sqlite3_changes(TheDb);
	TEST2(cnt, 1);
	SqlTimerPrint(_L("DELETE"), start, end);
	
	sqlite3_close(TheDb);
	TheDb = 0;
	}

///////////////////////////////////////////////////////////////////////////////////////

static void DoTests()
	{
	TheTest.Start(_L("Collect drive information"));
	CollectDriveInfo();
	
	TInt64 maxDrvSize = TheDriveInfo[TheBiggestDriveNo].iSizeMb * K1Mb;
	if(maxDrvSize <= K4Gb)
		{
		TheTest.Printf(_L("There is no drive bigger than 4Gb. The tests won't be executed.\r\n"));
		return;	
		}
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQLITE3-CT-4041 Create database, bigger than 4Gb"));
	CreateBigDbTest(K4Gb + 64 * K1Mb);

	TheTest.Next (_L("  @SYMTestCaseID:PDS-SQLITE3-CT-4042 64-bit database - simple operations test"));
	SimpleDbOperationsTest();

	(void)TheFs.Delete(TheDbName);
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);

	__UHEAP_MARK;

	TInt err = TheFs.Connect();
	TheTest(err == KErrNone);

	DeleteTestFiles();
	DoTests();
	DeleteTestFiles();

	__UHEAP_MARKEND;

	TheFs.Close();
	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
