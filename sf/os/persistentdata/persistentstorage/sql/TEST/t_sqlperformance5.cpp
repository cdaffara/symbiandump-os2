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
// Description: MDS harvesting performance test
//
#include <e32test.h>
#include <e32math.h>
#include <bautils.h>
#include <hal.h>
#include <sqldb.h>
#include "t_sqlcmdlineutil.h"

RTest 			TheTest(_L("t_sqlperformance5 test"));
RSqlDatabase 	TheDb;

_LIT(KDbName, 	"c:\\test\\t_sqlperformance5.db");

TFileName		TheDbFileName;
TBuf<200> 		TheTestTitle;
TCmdLineParams 	TheCmdLineParams(TCmdLineParams::EDbUtf16, 16384, 32);
TBuf8<200> 		TheSqlConfigString;

_LIT(KUtf8,  "UTF8 ");
_LIT(KUtf16, "UTF16");

const TInt KThumbnailCount = 60;
const TInt KMaxThumbnailSize = 40 * 1024;

const TInt KThumbnailSizes[KThumbnailCount] = 
		{
		//1    2      3      4      5      6      7      8      9      10
		22054, 24076, 33281, 24733, 33094, 31443, 29264, 28725, 31798, 29322, //1
		25002, 26926, 31097, 21988, 33659, 29081, 33050, 36857, 37686, 24034, //2
		21093, 28314, 20186, 27222, 28600, 32735, 27279, 31898, 31380, 36316, //3
		34295, 31642, 21829, 32912, 31584, 32557, 36601, 22744, 32808, 26130, //4
		31222, 21545, 35899, 22257, 25856, 31169, 34893, 23496, 23034, 30381, //5
		25810, 27123, 33442, 22275, 30260, 31028, 32415, 27345, 26614, 33704  //6
		};

TInt TheFastCounterFreq = 0;

TInt TheCreateDbTime = 0;
TInt TheCreateTablesTime = 0;
TInt TheBindParamsTime = 0;
TInt TheStmtExecTime = 0;
TInt TheStmtResetTime = 0;
TInt ThePopulateTempTableTime = 0;
TInt TheFlushTime = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////

void TestEnvDestroy()
	{
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
	ResetSoftHeapLimit();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TestEnvDestroy();
		TheTest.Printf(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TSqlRetCodeClass cl = SqlRetCodeClass(aValue);
		if(cl == ESqlDbError)
			{
			TPtrC errmsg = TheDb.LastErrorMessage();
			TheTest.Printf(_L("*** SQLite err=\"%S\"\r\n"), &errmsg);
			}
		TestEnvDestroy();
		TheTest.Printf(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void TestEnvInit()
	{
	RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);
	err = fs.MkDirAll(TheDbFileName);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	fs.Close();
	}

TInt TimeDiffUs(TUint32 aStartTicks, TUint32 aEndTicks)
	{
	if(TheFastCounterFreq == 0)
		{
		TEST2(HAL::Get(HAL::EFastCounterFrequency, TheFastCounterFreq), KErrNone);
		TheTest.Printf(_L("==Fast counter frequency: %d Hz\r\n"), TheFastCounterFreq);
		}
	TInt64 diffTicks = (TInt64)aEndTicks - (TInt64)aStartTicks;
	if(diffTicks < 0)
		{
		diffTicks = KMaxTUint32 + diffTicks + 1;
		}
	const TInt KMicroSecIn1Sec = 1000000;
	TInt us = (diffTicks * KMicroSecIn1Sec) / TheFastCounterFreq;
	return us;
	}

void PrintTime(const TDesC& aFmt, TUint32 aStartTicks, TUint32 aEndTicks)
	{
	TInt us = TimeDiffUs(aStartTicks, aEndTicks);
	TheTest.Printf(aFmt, us);
	}

//=============================================================================

/**
@SYMTestCaseID			PDS-SQL-CT-4205
@SYMTestCaseDesc		Thumbnail database performance test.
@SYMTestPriority		Medium
@SYMTestActions			The test executes statements to create the thumbnail database.  
@SYMTestExpectedResults The test must not fail
@SYMDEF					ou1cimx1#362240
*/
void CreateDb()
	{
	(void)RSqlDatabase::Delete(TheDbFileName);
	
	TUint32 fc1 = User::FastCounter();
	
	TInt err = TheDb.Create(TheDbFileName, &TheSqlConfigString);
	
	TUint32 fc2 = User::FastCounter();
	TheCreateDbTime = TimeDiffUs(fc1, fc2);
	
	TEST2(err, KErrNone);

	fc1 = User::FastCounter();
	
	err = TheDb.Exec(_L("CREATE TABLE ThumbnailInfo (Path TEXT COLLATE NOCASE,TNId INTEGER,Size INTEGER,Format INTEGER,TNPath TEXT COLLATE NOCASE,Width INTEGER,Height INTEGER,OrigWidth INTEGER,OrigHeight INTEGER,Flags INTEGER,VideoPosition INTEGER,Orientation INTEGER,humbFromPath INTEGER,Modified LARGEINT);"));
	TEST(err >= 0);

	err = TheDb.Exec(_L("CREATE TABLE ThumbnailInfoData(Data BLOB);"));
	TEST(err >= 0);
	
	err = TheDb.Exec(_L("CREATE TABLE ThumbnailDeleted(Path TEXT UNIQUE COLLATE NOCASE);"));
	TEST(err >= 0);
	
	err = TheDb.Exec(_L("CREATE INDEX idx1 ON ThumbnailInfo(Path, Size);"));
	TEST(err >= 0);
	
	err = TheDb.Exec(_L("CREATE INDEX idx4 ON ThumbnailDeleted(Path);"));
	TEST(err >= 0);

	err = TheDb.Exec(_L("CREATE TABLE ThumbnailVersion (Major INTEGER,Minor INTEGER,IMEI TEXT COLLATE NOCASE);"));
	TEST(err >= 0);

	fc2 = User::FastCounter();
	TheCreateTablesTime = TimeDiffUs(fc1, fc2);
	
	TheDb.Close();
	}

void PoulateTempTables(RSqlStatement& aStmt1, RSqlStatement& aStmt2)
	{
	HBufC8* thumbnailBuf = HBufC8::New(KMaxThumbnailSize);
	TEST(thumbnailBuf != NULL);
	TPtr8 thumbnailData = thumbnailBuf->Des();
	thumbnailData.SetLength(KMaxThumbnailSize);
	thumbnailData.Fill(TChar('A'));

	TUint32 fc3 = User::FastCounter();
	
	for(TInt i=0;i<KThumbnailCount;++i)
		{
		TUint32 fc1 = User::FastCounter();
	
		TInt paramIndex = aStmt1.ParameterIndex(_L(":Path"));
		TEST(paramIndex >= 0);
		TInt err = aStmt1.BindText(paramIndex, _L("c:\\test\\abcdefgh123456789.jpg"));
		TEST2(err, KErrNone);

		paramIndex = aStmt1.ParameterIndex(_L(":Width"));
		TEST(paramIndex >= 0);
		err = aStmt1.BindInt(paramIndex, 50);
		TEST2(err, KErrNone);

		paramIndex = aStmt1.ParameterIndex(_L(":Height"));
		TEST(paramIndex >= 0);
		err = aStmt1.BindInt(paramIndex, 40);
		TEST2(err, KErrNone);

		paramIndex = aStmt1.ParameterIndex(_L(":OrigWidth"));
		TEST(paramIndex >= 0);
		err = aStmt1.BindInt(paramIndex, 1000);
		TEST2(err, KErrNone);

		paramIndex = aStmt1.ParameterIndex(_L(":OrigHeight"));
		TEST(paramIndex >= 0);
		err = aStmt1.BindInt(paramIndex, 2000);
		TEST2(err, KErrNone);

		paramIndex = aStmt1.ParameterIndex(_L(":Format"));
		TEST(paramIndex >= 0);
		err = aStmt1.BindInt(paramIndex, 10);
		TEST2(err, KErrNone);

		paramIndex = aStmt1.ParameterIndex(_L(":Flags"));
		TEST(paramIndex >= 0);
		err = aStmt1.BindInt(paramIndex, 0x1E);
		TEST2(err, KErrNone);

		paramIndex = aStmt1.ParameterIndex(_L(":Size"));
		TEST(paramIndex >= 0);
		err = aStmt1.BindInt(paramIndex, 1200);
		TEST2(err, KErrNone);

		paramIndex = aStmt1.ParameterIndex(_L(":Orient"));
		TEST(paramIndex >= 0);
		err = aStmt1.BindInt(paramIndex, 2);
		TEST2(err, KErrNone);

		paramIndex = aStmt1.ParameterIndex(_L(":ThumbFromPath"));
		TEST(paramIndex >= 0);
		err = aStmt1.BindInt(paramIndex, 1);
		TEST2(err, KErrNone);

		paramIndex = aStmt1.ParameterIndex(_L(":Modified"));
		TEST(paramIndex >= 0);
		err = aStmt1.BindInt64(paramIndex, 3212398543392LL);
		TEST2(err, KErrNone);
		
		TUint32 fc2 = User::FastCounter();
		TheBindParamsTime += TimeDiffUs(fc1, fc2);

		fc1 = User::FastCounter();
		err = aStmt1.Exec();
		fc2 = User::FastCounter();
		TheStmtExecTime += TimeDiffUs(fc1, fc2);

		TEST2(err, 1);
		
		fc1 = User::FastCounter();
		err = aStmt1.Reset();
		fc2 = User::FastCounter();
		TheStmtResetTime += TimeDiffUs(fc1, fc2);
		
		TEST2(err, KErrNone);
		
		thumbnailData.SetLength(KThumbnailSizes[i]);

		fc1 = User::FastCounter();
		paramIndex = aStmt2.ParameterIndex(_L(":Data"));
		TEST(paramIndex >= 0);
		err = aStmt2.BindBinary(paramIndex, thumbnailData);
		TEST2(err, KErrNone);
		fc2 = User::FastCounter();
		TheBindParamsTime += TimeDiffUs(fc1, fc2);

		fc1 = User::FastCounter();
		err = aStmt2.Exec();
		fc2 = User::FastCounter();
		TheStmtExecTime += TimeDiffUs(fc1, fc2);
		
		TEST2(err, 1);
		
		fc1 = User::FastCounter();
		err = aStmt2.Reset();
		fc2 = User::FastCounter();
		TheStmtResetTime += TimeDiffUs(fc1, fc2);
		
		TEST2(err, KErrNone);
		}

	TUint32 fc4 = User::FastCounter();
	ThePopulateTempTableTime += TimeDiffUs(fc3, fc4);
	
	delete thumbnailBuf;
	}

void FlushTempTables()
	{
	TUint32 fc1 = User::FastCounter();
	
	TInt err = TheDb.Exec(_L("BEGIN TRANSACTION"));
	TEST(err >= 0);

	err = TheDb.Exec(_L("INSERT INTO ThumbnailInfo SELECT * FROM TempThumbnailInfo;"));
	TEST2(err, KThumbnailCount);

	err = TheDb.Exec(_L("INSERT INTO ThumbnailInfoData SELECT * FROM TempThumbnailInfoData;"));
	TEST2(err, KThumbnailCount);

	err = TheDb.Exec(_L("DELETE FROM TempThumbnailInfo;"));
	TEST(err >= 0);

	err = TheDb.Exec(_L("DELETE FROM TempThumbnailInfoData;"));
	TEST(err >= 0);

	err = TheDb.Exec(_L("COMMIT;"));
	TEST(err >= 0);

	TUint32 fc2 = User::FastCounter();
	TheFlushTime += TimeDiffUs(fc1, fc2);
	}

/**
@SYMTestCaseID			PDS-SQL-CT-4206
@SYMTestCaseDesc		Thumbnail database performance test.
@SYMTestPriority		Medium
@SYMTestActions			The test inserts 60 thumbnails with summary size of 1.7Mb into the thumbnail database.  
@SYMTestExpectedResults The test must not fail
@SYMDEF					ou1cimx1#362240
*/
void PopulateDb()
	{
	TInt dataToCommit = 0;
	for(TInt i=0;i<KThumbnailCount;++i)
		{
		dataToCommit += KThumbnailSizes[i]; 
		}
	TReal d = dataToCommit / 1024.0;
	TheTest.Printf(_L("==dataToCommit=%d bytes (%8.2lf Mb)\r\n"), dataToCommit, d);
	
	TInt err = TheDb.Open(TheDbFileName, &TheSqlConfigString);
	TEST2(err, KErrNone);

	err = TheDb.Exec(_L("CREATE TEMP TABLE TempThumbnailInfo (Path TEXT COLLATE NOCASE,TNId INTEGER,Size INTEGER,Format INTEGER,TNPath TEXT COLLATE NOCASE,Width INTEGER,Height INTEGER,OrigWidth INTEGER,OrigHeight INTEGER,Flags INTEGER,VideoPosition INTEGER,Orientation INTEGER,ThumbFromPath INTEGER,Modified LARGEINT);"));
	TEST(err >= 0);
	
	err = TheDb.Exec(_L("CREATE TEMP TABLE TempThumbnailInfoData (Data BLOB);"));
	TEST(err >= 0);
		
	RSqlStatement stmt1;
	err = stmt1.Prepare(TheDb, _L("INSERT INTO TempThumbnailInfo(Path,Size,Format,Width,Height,OrigWidth,OrigHeight,Flags,Orientation,ThumbFromPath,Modified) VALUES (:Path,:Size,:Format,:Width,:Height,:OrigWidth,:OrigHeight,:Flags,:Orient,:ThumbFromPath,:Modified);"));
	TEST2(err, KErrNone);

	RSqlStatement stmt2;
	err = stmt2.Prepare(TheDb, _L("INSERT INTO TempThumbnailInfoData (Data) VALUES (:Data);"));
	TEST2(err, KErrNone);
	
	PoulateTempTables(stmt1, stmt2);
	FlushTempTables();
	
	stmt2.Close();
	stmt1.Close();
	TheDb.Close();

	TheTest.Printf(_L("==Create database, time=%d microseconds\r\n"), TheCreateDbTime);
	TheTest.Printf(_L("==Create tables, time=%d microseconds\r\n"), TheCreateTablesTime);
	TheTest.Printf(_L("==Bind parameters time, time=%d microseconds\r\n"), TheBindParamsTime);
	TheTest.Printf(_L("==Temp tables, statement exec, time=%d microseconds\r\n"), TheStmtExecTime);
	TheTest.Printf(_L("==Temp tables, statement reset, time=%d microseconds\r\n"), TheStmtResetTime);
	TheTest.Printf(_L("==Populate temp tables, time=%d microseconds\r\n"), ThePopulateTempTableTime);
	TheTest.Printf(_L("==Copy temp tables to main tables, time=%d microseconds\r\n"), TheFlushTime);
	}

void DoTestsL()
	{
	TheTestTitle.Format(_L("@SYMTestCaseID:PDS-SQL-CT-4205 Create database, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Start(TheTestTitle);
	CreateDb();

	TheTestTitle.Format(_L("@SYMTestCaseID:PDS-SQL-CT-4206 Populate database, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Next(TheTestTitle);
	PopulateDb();
	
	(void)RSqlDatabase::Delete(TheDbFileName);
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;

	GetCmdLineParamsAndSqlConfigString(TheTest, _L("t_sqlperformance5"), TheCmdLineParams, TheSqlConfigString);
	PrepareDbName(KDbName, TheCmdLineParams.iDriveName, TheDbFileName);
	SetSoftHeapLimit(TheCmdLineParams.iSoftHeapLimitKb);
	
	TheTest.Printf(_L("==Databases: %S\r\n"), &TheDbFileName); 
	
	TestEnvDestroy();
	TestEnvInit();
	TRAPD(err, DoTestsL());
	TestEnvDestroy();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
