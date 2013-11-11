// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <hal.h>
#include "SqlUtil.h"

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlcompact1 test"));

RSqlDatabase TheDb;

const TInt KTextLen = 400;
TBuf<KTextLen> TheText;
TBuf<KTextLen + 100> TheSqlBuf;

_LIT(KTestDir, "c:\\test\\");
_LIT(KDbName1, "c:\\test\\t_sqlcompact1_1.db");
_LIT(KDbName2, "c:\\test\\t_sqlcompact1_2.db");
_LIT(KDbName3, "c:\\test\\t_sqlcompact1_3.db");
_LIT(KDbName4, "c:\\test\\t_sqlcompact1_4.db");

_LIT(KAttachName1, "UOOO");
_LIT(KAttachName2, "FOOO");
_LIT(KAttachName3, "AOOO");
_LIT(KAttachName4, "EOOO");

///////////////////////////////////////////////////////////////////////////////////////

void DestroyTestEnv()
	{
	TheDb.Close();
	(void)RSqlDatabase::Delete(KDbName4);
	(void)RSqlDatabase::Delete(KDbName3);
	(void)RSqlDatabase::Delete(KDbName2);
	(void)RSqlDatabase::Delete(KDbName1);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Test failure. Boolean expression evaluates to false.\r\n"));
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

//t_sqlcompact1 timeouts in WDP builds.
//This function is used to check whether the time limit is reaqched or not.
TBool IsTimeLimitReached()
	{
	struct TStartTime
		{
		TStartTime()
			{
			iTime.HomeTime();
			}
		TTime iTime;
		};
	
	static TStartTime startTime; 
	const TInt KTestTimeLimit = 100;//seconds
	
	TTime currTime;
	currTime.HomeTime();
	
	TTimeIntervalSeconds s;
	TInt err = currTime.SecondsFrom(startTime.iTime, s);
	TEST2(err, KErrNone);
	return s.Int() > KTestTimeLimit;
	}

void GetHomeTimeAsString(TDes& aStr)
	{
	TTime time;
	time.HomeTime();
	TDateTime dt = time.DateTime();
	aStr.Format(_L("%02d:%02d:%02d.%06d"), dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond());
	}

void CreateTestEnv()
    {
    RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);

	err = fs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	fs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////

void ReplaceDb(const TDesC& aDbName, TInt aPageSize)
	{
	(void)RSqlDatabase::Delete(aDbName);
	_LIT8(KConfigStr, "compaction=manual;page_size=");
	TBuf8<50> config;
	config.Copy(KConfigStr);
	config.AppendNum(aPageSize);
	TInt err = TheDb.Create(aDbName, &config);
	TEST2(err, KErrNone);
	TheDb.Close();
	}

void CreateTable(const TDesC& aDbName)
	{
	TInt err = TheDb.Open(aDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(I INTEGER, T TEXT)"));
	TEST(err >= 0);
	TheDb.Close();
	}

void InsertRecords(const TDesC& aDbName)
	{
	TInt err = TheDb.Open(aDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("BEGIN TRANSACTION"));
	TEST(err >= 0);
	TheText.SetLength(TheText.MaxLength());
	TheText.Fill(TChar('A'));
	for(TInt i=0;i<100;++i)
		{
		TheSqlBuf.Format(_L("INSERT INTO A VALUES(%d, '%S')"), i + 1, &TheText);
		err = TheDb.Exec(TheSqlBuf);
		TEST2(err, 1);
		}
	err = TheDb.Exec(_L("COMMIT TRANSACTION"));
	TEST(err >= 0);
	TheDb.Close();
	}

TInt DeleteRecords(const TDesC& aDbName, TInt aPageCount, TInt aPageSize)
	{
	TInt freePageCount = -1;
	TInt err = TheDb.Open(aDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("BEGIN TRANSACTION"));
	TEST(err >= 0);
	for(TInt i=0;;++i)
		{
		TheSqlBuf.Format(_L("DELETE FROM A WHERE I=%d"), i + 1);
		err = TheDb.Exec(TheSqlBuf);
		TEST2(err, 1);
		RSqlDatabase::TSize s;
		err = TheDb.Size(s);
		TEST2(err, KErrNone);
		freePageCount = s.iFree / aPageSize;
		if(freePageCount >= aPageCount)
			{
			break;	
			}
		}
	err = TheDb.Exec(_L("COMMIT TRANSACTION"));
	TEST(err >= 0);
	TheDb.Close();
	return freePageCount;
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4072
@SYMTestCaseDesc		Manual compaction on attached databases with different page size.
						The test creates couple of databases with manual compaction and 
						different page sizes, then inserts some records and deletes part of
						the just inserted records thus making some free pages.
						The test opens the first database and attaches all other databases the the first one.
						Then the test checks that RSqlDatabase::Size() returns correct information
						about the free database space. The test runs the manual compaction on the
						databases and checks again that the free database space is reported correctly.
@SYMTestPriority		Medium
@SYMTestActions			Manual compaction on attached databases with different page size.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10405
                        REQ10407
*/
void CompactDbTest1()
	{
	const TPtrC KDbName[] = 	{KDbName1(), 	KDbName2(), 	KDbName3(), 	KDbName4()};
	const TPtrC KDbAttachName[]={KAttachName1(),KAttachName2(), KAttachName3(), KAttachName4()};
	const TInt KDbPageSize[] = 	{8192, 			1024, 			4096, 			2048};
	const TInt KFreePageCount[]={9, 			30, 			17, 			7};
		  TInt freePageCount[] ={0, 0, 0, 0};
	const TInt KSize = sizeof(KDbName) / sizeof(KDbName[0]);
	
	TInt i;
	
	TBuf<50> timeBuf;
	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time1: %S\r\n"), &timeBuf);
	
	//Create databases, tables, insert records, delete part of the just inserted records.
	for(i=0;i<KSize;++i)
		{
		ReplaceDb(KDbName[i], KDbPageSize[i]);
		CreateTable(KDbName[i]);
		InsertRecords(KDbName[i]);
		freePageCount[i] = DeleteRecords(KDbName[i], KFreePageCount[i], KDbPageSize[i]);
		}

	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time2: %S\r\n"), &timeBuf);
	
	//Open the first database, attach all others.
	TInt err = TheDb.Open(KDbName1());
	TEST2(err, KErrNone);
	for(i=0;i<KSize;++i)
		{
		err = TheDb.Attach(KDbName[i], KDbAttachName[i]);
		TEST2(err, KErrNone);
		}

	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time3: %S\r\n"), &timeBuf);
	
	//Check the size of the main database.
	RSqlDatabase::TSize size;
	err = TheDb.Size(size);
	TEST2(err, KErrNone);
	TEST2((size.iFree / KDbPageSize[0]), freePageCount[0]);
		
	//For all attached database: check the size of the database, compact, check the size again.
	for(i=0;i<KSize;++i)
		{
		err = TheDb.Size(size, KDbAttachName[i]);
		TEST2(err, KErrNone);
		TEST2((size.iFree / KDbPageSize[i]), freePageCount[i]);
		
		const TInt KCompactPageCount = 3;
		TInt rc = TheDb.Compact(KCompactPageCount * KDbPageSize[i], KDbAttachName[i]);
		TInt expected = KCompactPageCount * KDbPageSize[i];
		TEST2(rc, expected);
		err = TheDb.Size(size, KDbAttachName[i]);
		TEST2(err, KErrNone);
		TInt count = size.iFree / KDbPageSize[i];
		expected = freePageCount[i] - KCompactPageCount;
		TEST2(count, expected);
		}
	
	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time4: %S\r\n"), &timeBuf);
	
	//Detach databases and close the main database.
	for(i=0;i<KSize;++i)
		{
		err = TheDb.Detach(KDbAttachName[i]);
		TEST2(err, KErrNone);
		}
	TheDb.Close();

	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time5: %S\r\n"), &timeBuf);
	
	//Cleanup.
	for(i=0;i<KSize;++i)
		{
		(void)RSqlDatabase::Delete(KDbName[i]);
		}
	}

//Creates a test database (with KDbName1 name). Inserts aRecordCount records.
//The page size is specified in aPageSize parameter. But practically the page size is always 1024 bytes.
//The record size is such that there is only one record per page.
void PrepareDb(TInt aPageSize, TInt aRecordCount, TBool aManualCompaction = EFalse)
	{
	//Create the database
	(void)RSqlDatabase::Delete(KDbName1);
	_LIT8(KConfigStr, "page_size=");
	TBuf8<100> config;
	config.Copy(KConfigStr);
	config.AppendNum(aPageSize);
	if(aManualCompaction)
		{
		config.Append(_L(";compaction=manual;"));
		}
	TInt err = TheDb.Create(KDbName1, &config);
	TEST2(err, KErrNone);
	
	err = TheDb.Exec(_L("BEGIN TRANSACTION"));
	TEST(err >= 0);
	err = TheDb.Exec(_L("CREATE TABLE A(I INTEGER, T TEXT)"));
	TEST(err >= 0);
	//Insert records
	TheText.SetLength(TheText.MaxLength());
	TheText.Fill(TChar('A'));
	for(TInt i=0;i<aRecordCount;++i)
		{
		TheSqlBuf.Format(_L("INSERT INTO A VALUES(%d, '%S')"), i + 1, &TheText);
		err = TheDb.Exec(TheSqlBuf);
		TEST2(err, 1);
		}
	err = TheDb.Exec(_L("COMMIT TRANSACTION"));
	TEST(err >= 0);
	//Delete all records making a lot of free pages. This operation should kick-off the background compaction
	err = TheDb.Exec(_L("DELETE FROM A WHERE 1"));
	TEST2(err, aRecordCount);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4073
@SYMTestCaseDesc		Background compaction steps test.
						The test creates a database with background compaction mode, 
						then inserts records and deletes all of them. The count of records is such that when
						the records get deleted, the number of the free pages is very big and all free pages cannot
						be removed for just one compaction step.
						The test waits for ("compaction interval"/10 ms) time and checks that no compaction
						step has been run by the server during the pause and the free space size is the same as before the
						pause. Then the test waits for ("compaction interval" + "compaction step") time and checks that
						the background compaction step really happened and removed only part of the free pages.
						The same test is repeated again and the same check is performed again.
@SYMTestPriority		Medium
@SYMTestActions			Background compaction steps test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
                        REQ10272
*/
void CompactDbTest2()
	{
	TBuf<50> timeBuf;
	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time1: %S\r\n"), &timeBuf);
	
	const TInt KPageSize = 1024;
	//Number of records to be added and removed from database. Need to be increased when testing on a faster 
	// hardware, otherwise at fastest case the background compaction could be finished in just 1 step.
	const TInt KRecordCount = 2000;	
	PrepareDb(KPageSize, KRecordCount);

	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time2: %S\r\n"), &timeBuf);
	
	//Check the free space-1
	RSqlDatabase::TSize size1;
	TInt err = TheDb.Size(size1);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("===Free space before compaction, pages=%d\r\n"), size1.iFree / KPageSize);
	TEST(size1.iSize >= (KRecordCount * KPageSize));
	
	//Wait KSqlCompactStepIntervalMs/10 ms. The background compaction should not be kicked-off.
	TTime time1;
	time1.HomeTime();
	User::After((KSqlCompactStepIntervalMs / 10) * 1000);
	TTime time2;
	time2.HomeTime();
	TTimeIntervalMicroSeconds intervalUs = time2.MicroSecondsFrom(time1);	
	//Check the free space-2
	RSqlDatabase::TSize size2;
	err = TheDb.Size(size2);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("=== Wait time: %ld ms. Free space after compaction-1, pages=%d\r\n"), intervalUs.Int64() / 1000 ,size2.iFree / KPageSize);
	if(intervalUs > KSqlCompactStepIntervalMs * 1000)
		{
		TEST(size2.iFree <= size1.iFree);
		}
	else
		{
		TEST(size2.iFree == size1.iFree);
		}

	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time3: %S\r\n"), &timeBuf);
	
	//Wait (KSqlCompactStepIntervalMs + KSqlCompactStepLengthMs) ms. During the pause only part of the free pages
	//should be removed (whatever can be completed for KSqlCompactStepLengthMs ms).
	User::After((KSqlCompactStepIntervalMs + KSqlCompactStepLengthMs) * 1000);
	//Check the free space-3
	RSqlDatabase::TSize size3;
	err = TheDb.Size(size3);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("===Free space after compaction-2, pages=%d\r\n"), size3.iFree / KPageSize);
	if(size3.iFree == 0)
		{
		TheTest.Printf(_L("WARNING: Background compaction finished in 1 step. Initial number of records need to be increased.\r\n"));
		}
	TEST(size3.iFree > 0 && size3.iFree < size2.iFree);

	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time4: %S\r\n"), &timeBuf);
	
	//Wait another (KSqlCompactStepIntervalMs + KSqlCompactStepLengthMs) ms. During the pause only part of the free pages
	//should be removed (whatever can be completed for KSqlCompactStepLengthMs ms).
	User::After((KSqlCompactStepIntervalMs + KSqlCompactStepLengthMs) * 1000);
	//Check the free space-4
	RSqlDatabase::TSize size4;
	err = TheDb.Size(size4);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("===Free space after compaction-3, pages=%d\r\n"), size4.iFree / KPageSize);
	TEST((size4.iFree > 0 && size4.iFree < size3.iFree) || (size4.iFree == 0));

	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time5: %S\r\n"), &timeBuf);
	
	//Cleanup
	TheDb.Close();
	(void)RSqlDatabase::Delete(KDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4074
@SYMTestCaseDesc		Background compaction timer test.
						The test creates a database with background compaction mode, 
						then inserts records and deletes all of them. The count of records is such that when
						the records get deleted, the number of the free pages is very big and all free pages cannot
						be removed for just one compaction step.
						Then the test executes a set of operations with the server. The amount of time needed for the
						operations to be executed is bigger than the ("compaction interval" + "compaction step") time.
						No compaction step should be executed during that time, because every operation resets the background 
						compaction timer.
@SYMTestPriority		Medium
@SYMTestActions			Background compaction timer test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
                        REQ10272
*/
void CompactDbTest3()
	{
	TBuf<50> timeBuf;
	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time1: %S\r\n"), &timeBuf);
	
	const TInt KPageSize = 1024;
	const TInt KRecordCount = 1000;
	PrepareDb(KPageSize, KRecordCount);

	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time2: %S\r\n"), &timeBuf);
	
	//Check the free space-1
	RSqlDatabase::TSize size1;
	TInt err = TheDb.Size(size1);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("===Free space before operations, pages=%d. Db.Size=%d, Db.Free=%d\r\n"), size1.iFree / KPageSize, size1.iSize, size1.iFree);
	TEST(size1.iSize >= (KRecordCount * KPageSize));

	//Execute a set of operations. The time needed for the operations to complete is bigger than
	//(KSqlCompactStepIntervalMs + KSqlCompactStepLengthMs) ms
	TInt freq = 0;
	TEST2(HAL::Get(HAL::EFastCounterFrequency, freq), KErrNone);
	TUint32 begin = User::FastCounter();
	TInt count = 0;
	TInt time = -1;
	for(;;++count)
		{
		err = TheDb.Exec(_L("SELECT COUNT(*) FROM A"));
		TEST(err >= 0);
		TUint32 current = User::FastCounter();
		TInt64 diffTicks = (TInt64)current - (TInt64)begin;
		if(diffTicks < 0)
			{
			diffTicks = KMaxTUint32 + diffTicks + 1;
			}
		const TInt KMicroSecIn1Sec = 1000000;
		TInt32 us = (diffTicks * KMicroSecIn1Sec) / freq;
		time = us / 1000;
		if(time > ((KSqlCompactStepIntervalMs + KSqlCompactStepLengthMs)))
			{
			break;	
			}
		}
	
	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time3: %S\r\n"), &timeBuf);
	
	//Check the free space-2
	RSqlDatabase::TSize size2;
	err = TheDb.Size(size2);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("===%d operations completed for %d ms\r\n"), count, time);
	TheTest.Printf(_L("===Free space after operations, pages=%d\r\n"), size2.iFree / KPageSize);
	TEST(size1.iFree == size2.iFree);
	
	//Cleanup
	TheDb.Close();
	(void)RSqlDatabase::Delete(KDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4103
@SYMTestCaseDesc		Big manual compaction test.
						The test creates a database with 1000 free pages, then calls 
						RSqlDatabase::Compact(RSqlDatabase::EMaxCompaction).
@SYMTestPriority		Medium
@SYMTestActions			Big manual compaction test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
                        REQ10272
*/
void ManualCompactTest()
	{
	TBuf<50> timeBuf;
	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time1: %S\r\n"), &timeBuf);
	
	//Create a database with 1000 free pages
	const TInt KPageSize = 1024;
	const TInt KRecordCount = 1000;
	PrepareDb(KPageSize, KRecordCount, ETrue);//create the database with manual compaction mode
	
	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time2: %S\r\n"), &timeBuf);
	
	//Check the free space-1
	RSqlDatabase::TSize size1;
	TInt err = TheDb.Size(size1);
	TEST2(err, KErrNone);
	const TInt KFreePageCount = size1.iFree / KPageSize;
	TheTest.Printf(_L("===Free space before operations, pages=%d\r\n"), KFreePageCount);
	TEST(size1.iSize >= (KRecordCount * KPageSize));
	//Compact
	err = TheDb.Compact(RSqlDatabase::EMaxCompaction);
	TEST2(err, size1.iFree);
	
	GetHomeTimeAsString(timeBuf);
	TheTest.Printf(_L("===Time3: %S\r\n"), &timeBuf);
	
	//Cleanup
	TheDb.Close();
	(void)RSqlDatabase::Delete(KDbName1);
	}

void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4072 Manual Compact() - attached databases, different page sizes"));	
	CompactDbTest1();

	if(IsTimeLimitReached())
		{
		TheTest.Printf(_L("===Test timeout!\r\n"));
		return;
		}
	
	TheTest.Next( _L(" @SYMTestCaseID:SYSLIB-SQL-UT-4073 Background compaction steps test"));	
	CompactDbTest2();

	if(IsTimeLimitReached())
		{
		TheTest.Printf(_L("===Test timeout!\r\n"));
		return;
		}
	
	TheTest.Next( _L(" @SYMTestCaseID:SYSLIB-SQL-UT-4074 Background compaction timer test"));	
	CompactDbTest3();

	if(IsTimeLimitReached())
		{
		TheTest.Printf(_L("===Test timeout!\r\n"));
		return;
		}
	
	TheTest.Next( _L(" @SYMTestCaseID:SYSLIB-SQL-UT-4103 Big manual compaction test"));	
	ManualCompactTest();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
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
