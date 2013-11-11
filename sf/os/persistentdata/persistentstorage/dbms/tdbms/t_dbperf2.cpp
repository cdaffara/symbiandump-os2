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

#include <d32dbms.h>
#include <s32file.h>
#include <e32test.h>
#include <e32math.h>
#include <s32mem.h>
#include <hal.h>

static RTest TheTest(_L("t_dbperf2"));
static CTrapCleanup* TheTrapCleanup;
static RFs TheFs;
static RDbs TheDbs;
static RDbNamedDatabase TheDatabase;
static RDbRowSet TheRowSet;
static RFile TheTestFile;
static TFileName TheDatabaseFileName;
static TFileName TheLogFileName;
static TFileName TheTestFileName;
static TParse TheParse;

#define COUNT_OF(array) (sizeof(array)/sizeof(array[0]))

const TInt KTestRecordCount = 400;

const TInt KTextColSize = 200;//Change KCreateTestTableSql string too!

_LIT(KCreateTestTableSql, "CREATE TABLE A(I1 INTEGER, I2 DOUBLE, I3 VARCHAR(200))");
_LIT(KCreateIndexSql, "CREATE UNIQUE INDEX IDX ON A(I1)");

const TInt KColSize2 = 500;
_LIT(KCreateTestTableSql2, "CREATE TABLE A(I1 INTEGER, I2 LONG VARCHAR, I3 LONG VARBINARY)");

const TChar KBufChar('O');
TBuf8<KColSize2> TheBinRndData;
TBuf16<KColSize2> TheTextRndData;

//////////////////////////////////////////////////////

static TInt TheCounterFreq = -10000000;
const TInt KMicroSecIn1Sec = 1000000;

TUint32 CalcTickDiff(TUint32 aStartTicks, TUint32 aEndTicks)
	{
	TInt64 diffTicks = (TInt64)aEndTicks - (TInt64)aStartTicks;
	if(diffTicks < 0)
		{
		diffTicks = KMaxTUint32 + diffTicks + 1;
		}
	return (TUint32)diffTicks;
	}

//Prints aFastCount parameter (converted to us)
void PrintFcDiffAsUs(const TDesC& aFormatStr, TUint32 aFastCount)
	{
	double v = ((double)aFastCount * KMicroSecIn1Sec) / (double)TheCounterFreq;
	TInt v2 = (TInt)v;
	TheTest.Printf(aFormatStr, v2);
	}

//Prints aFastCount parameter (converted to us) and the records count
void PrintFcDiffAsUs2(const TDesC& aFormatStr, TInt aRecCnt, TUint32 aFastCount)
	{
	double v = ((double)aFastCount * KMicroSecIn1Sec) / (double)TheCounterFreq;
	TInt v2 = (TInt)v;
	TheTest.Printf(aFormatStr, aRecCnt, v2);
	}

//////////////////////////////////////////////////////

enum TRowSetType {EViewRowSet, ETableRowSet};
const TRowSetType KRowSetTypes[] = {EViewRowSet/*, ETableRowSet*/}; //Excluding ETableRowSet means that the test requires less time.
const TPtrC KRowSetTypeStr[] = {_L("RDbView.")/*, _L("RDbTable.")*/};//The coverage is not affected that much.

//////////////////////////////////////////////////////

const RDbRowSet::TAccess KAccessModes[] = {RDbRowSet::EInsertOnly, RDbRowSet::EUpdatable};
const TPtrC KAccessModeStr[] = {_L("Insert only."), _L("Updatable.")};

//////////////////////////////////////////////////////

enum TUpdDirection {EUpdBackward, EUpdForward};
const TUpdDirection KUpdDirectionTypes[] = {EUpdBackward, EUpdForward};
const TPtrC KUpdDirectionTypeStr[] = {_L("Backward."), _L("Forward.")};

//////////////////////////////////////////////////////

enum TCommitRecCnt {ECommit_1_Rec = 1, ECommit_Cnt_Rec = 20, ECommit_All_Rec = KTestRecordCount};
const TCommitRecCnt KCommitTypes[] = {ECommit_1_Rec, ECommit_Cnt_Rec, ECommit_All_Rec};
const TPtrC KCommitTypeStr[] = {_L("Commit after 1 op."), _L("Commit after 20 ops."), _L("Commit at end.")};

///////////////////////////////////////////////////////////////////////////////////////

//Delete "aFullName" file.
static void DeleteFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
			{
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone) 
				{
				TheTest.Printf(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
				}
			err = fsSession.Delete(aFullName);
			if(err != KErrNone) 
				{
				TheTest.Printf(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		TheTest.Printf(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	}

///////////////////////////////////////////////////////////////////////////////////////

static void CloseAll()
	{
	TheRowSet.Close();
	TheDatabase.Close();
	TheDbs.Close();
	TheTestFile.Close();
	TheFs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Tests macros and functions.
//If (!aValue) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		CloseAll();
		DeleteFile(TheDatabaseFileName);
		DeleteFile(TheTestFileName);
		TheTest(EFalse, aLine);
		}
	}
//If (aValue != aExpected) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TheTest.Printf(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		CloseAll();
		DeleteFile(TheDatabaseFileName);
		DeleteFile(TheTestFileName);
		TheTest(EFalse, aLine);
		}
	}
//Use these to test conditions.
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void PrintFileSize()
	{
	RDbDatabase::TSize s = TheDatabase.Size();
	TheTest.Printf(_L("####FileSize: %d\r\n"), s.iSize);
	}

///////////////////////////////////////////////////////////////////////////////////////

void DbRowSetInsertTestL(TRowSetType aRowSetType, RDbRowSet::TAccess aAccessMode, TCommitRecCnt aCommitRecCnt)
	{
	TBuf<KTextColSize> textColVal;
	for(TInt ii=23000;ii<(23000+KTextColSize);++ii)
		{
		textColVal.Append(TChar(ii));
		}
	for(TInt compaction=0;compaction<2;++compaction)
		{
		TheTest.Printf(compaction ? _L("Compaction:Yes.\r\n") : _L("Compaction:No.\r\n"));
		TheRowSet.Close();
		TInt err;
		err = TheDatabase.Execute(_L("DELETE FROM A"));
		TEST(err >= 0);
		TUint32 fc = User::FastCounter();
		if(aRowSetType == EViewRowSet)
			{
			RDbView view;
			err = view.Prepare(TheDatabase, _L("SELECT * FROM A"), aAccessMode);
			TEST2(err, KErrNone);
			err = view.EvaluateAll();
			TEST2(err, KErrNone);
			TheRowSet = view;
			}
		else
			{
			RDbTable tbl;
			err = tbl.Open(TheDatabase, _L("A"), aAccessMode);
			TEST2(err, KErrNone);
			TheRowSet = tbl;
			}
		TUint32 prepFc = CalcTickDiff(fc, User::FastCounter());
		TUint32 insFc = 0;
		TUint32 setcolFc = 0;
		TUint32 putFc = 0;
		TUint32 commitFc = 0;
		TUint32 totalFc = 0;
		fc = User::FastCounter();
		for(TInt i=0,count=0;i<KTestRecordCount;++i)
			{
			TUint32 tmpFc;
			if(aCommitRecCnt != ECommit_1_Rec && count == 0)
				{
				tmpFc = User::FastCounter();
				err = TheDatabase.Begin();	
				TEST2(err, KErrNone);
				commitFc += CalcTickDiff(tmpFc, User::FastCounter());
				}
			tmpFc = User::FastCounter();
			TheRowSet.InsertL();
			insFc += CalcTickDiff(tmpFc, User::FastCounter());
			tmpFc = User::FastCounter();
			TheRowSet.SetColL(1, i);
			TheRowSet.SetColL(2, i * 10.1234);
			TheRowSet.SetColL(3, textColVal);
			setcolFc += CalcTickDiff(tmpFc, User::FastCounter());
			tmpFc = User::FastCounter();
			TheRowSet.PutL();
			putFc += CalcTickDiff(tmpFc, User::FastCounter());
			if(aCommitRecCnt != ECommit_1_Rec && ++count == aCommitRecCnt)
				{
				tmpFc = User::FastCounter();
				count = 0;
				err = TheDatabase.Commit();	
				TEST2(err, KErrNone);
				commitFc += CalcTickDiff(tmpFc, User::FastCounter());
				if(compaction)
					{
					err = TheDatabase.Compact();
					TEST2(err, KErrNone);
					}
				}
			}
		totalFc = CalcTickDiff(fc, User::FastCounter());
		PrintFcDiffAsUs(_L("###Prepare,time=%d us\r\n"), prepFc);
		PrintFcDiffAsUs(_L("###InsertL,time=%d us\r\n"), insFc);
		PrintFcDiffAsUs(_L("###SetColL,time=%d us\r\n"), setcolFc);
		PrintFcDiffAsUs(_L("###PutL,   time=%d us\r\n"), putFc);
		PrintFcDiffAsUs(_L("###Commit, time=%d us\r\n"), commitFc);
		PrintFcDiffAsUs(_L("###Total,  time=%d us\r\n"), totalFc);
		TheRowSet.Close();
		PrintFileSize();
		//Check
		if(aAccessMode != RDbRowSet::EInsertOnly)
			{
			RDbView view;
			err = view.Prepare(TheDatabase, _L("SELECT * FROM A"), aAccessMode);
			TEST2(err, KErrNone);
			err = view.EvaluateAll();
			TEST2(err, KErrNone);
			TInt count = view.CountL();
			view.Close();
			TEST2(count, KTestRecordCount);
			}
		}//end of - for(TInt compaction=0;compaction<2;++compaction)
	}

void FirstRecL(TUpdDirection aUpdDirection)
	{
	TBool rc = EFalse;
	switch(aUpdDirection)
		{
		case EUpdBackward:
			rc = TheRowSet.LastL();
			break;
		case EUpdForward:
		default:
			rc = TheRowSet.FirstL();
			break;
		}
	TEST(rc);
	}

void NextRecL(TUpdDirection aUpdDirection)
	{
	TBool rc = EFalse;
	switch(aUpdDirection)
		{
		case EUpdBackward:
			rc = TheRowSet.PreviousL();
			break;
		case EUpdForward:
		default:
			rc = TheRowSet.NextL();
			break;
		}
	TEST(rc);
	}

void DbRowSetUpdateTestL(TRowSetType aRowSetType, TUpdDirection aUpdDirection, TCommitRecCnt aCommitRecCnt)
	{
	TBuf<KTextColSize> textColVal;
	for(TInt ii=33000;ii<(33000+KTextColSize);++ii)
		{
		textColVal.Append(TChar(ii));
		}
		
	for(TInt compaction=0;compaction<2;++compaction)
		{
		TheTest.Printf(compaction ? _L("Compaction:Yes.\r\n") : _L("Compaction:No.\r\n"));
		TheRowSet.Close();
		TInt err;
		TUint32 fc = User::FastCounter();
		if(aRowSetType == EViewRowSet)
			{
			RDbView view;
			err = view.Prepare(TheDatabase, _L("SELECT * FROM A WHERE I1 >= 0 AND I1 <= 1000000"), RDbRowSet::EUpdatable);
			TEST2(err, KErrNone);
			err = view.EvaluateAll();
			TEST2(err, KErrNone);
			TheRowSet = view;
			}
		else
			{
			RDbTable tbl;
			err = tbl.Open(TheDatabase, _L("A"), RDbRowSet::EUpdatable);
			TEST2(err, KErrNone);
			TheRowSet = tbl;
			}
		TInt cnt = TheRowSet.CountL();
		TEST2(cnt, KTestRecordCount);
		TUint32 prepFc = CalcTickDiff(fc, User::FastCounter());
		TUint32 insFc = 0;
		TUint32 setcolFc = 0;
		TUint32 putFc = 0;
		TUint32 commitFc = 0;
		TUint32 totalFc = 0;
		fc = User::FastCounter();
		for(TInt i=0,count=0;i<KTestRecordCount;++i)
			{
			TRAP(err, (i == 0 ? FirstRecL(aUpdDirection) : NextRecL(aUpdDirection)));
			TEST2(err, KErrNone);
			TUint32 tmpFc;
			if(aCommitRecCnt != ECommit_1_Rec && count == 0)
				{
				tmpFc = User::FastCounter();
				err = TheDatabase.Begin();	
				TEST2(err, KErrNone);
				commitFc += CalcTickDiff(tmpFc, User::FastCounter());
				}
			tmpFc = User::FastCounter();
			TheRowSet.UpdateL();
			insFc += CalcTickDiff(tmpFc, User::FastCounter());
			tmpFc = User::FastCounter();
			TheRowSet.SetColL(2, i * 20.0);
			TheRowSet.SetColL(3, textColVal);
			setcolFc += CalcTickDiff(tmpFc, User::FastCounter());
			tmpFc = User::FastCounter();
			TheRowSet.PutL();
			putFc += CalcTickDiff(tmpFc, User::FastCounter());
			if(aCommitRecCnt != ECommit_1_Rec && ++count == aCommitRecCnt)
				{
				tmpFc = User::FastCounter();
				count = 0;
				err = TheDatabase.Commit();	
				TEST2(err, KErrNone);
				commitFc += CalcTickDiff(tmpFc, User::FastCounter());
				if(compaction)
					{
					err = TheDatabase.Compact();
					TEST2(err, KErrNone);
					}
				}
			}
		totalFc = CalcTickDiff(fc, User::FastCounter());
		PrintFcDiffAsUs(_L("###Prepare,time=%d us\r\n"), prepFc);
		PrintFcDiffAsUs(_L("###UpdateL,time=%d us\r\n"), insFc);
		PrintFcDiffAsUs(_L("###SetColL,time=%d us\r\n"), setcolFc);
		PrintFcDiffAsUs(_L("###PutL,   time=%d us\r\n"), putFc);
		PrintFcDiffAsUs(_L("###Commit, time=%d us\r\n"), commitFc);
		PrintFcDiffAsUs(_L("###Total,  time=%d us\r\n"), totalFc);
		TheRowSet.Close();
		PrintFileSize();
		}//end of - for(TInt compaction=0;compaction<2;++compaction)
	}

///////////////////////////////////////////////////////////////////////////////////////

void CreateDatabase()
	{
	TInt err = TheDatabase.Replace(TheFs, TheDatabaseFileName);
	TEST2(err, KErrNone);
	TheDatabase.Close();
	err = TheDbs.Connect();
	TEST2(err, KErrNone);
	err = TheDatabase.Open(TheDbs, TheDatabaseFileName);
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(KCreateTestTableSql);
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(KCreateIndexSql);
	TEST2(err, KErrNone);
	}

void CreateAndFillDatabase()
	{
	TInt err = TheDatabase.Replace(TheFs, TheDatabaseFileName);
	TEST2(err, KErrNone);
	TheDatabase.Close();
	err = TheDbs.Connect();
	TEST2(err, KErrNone);
	err = TheDatabase.Open(TheDbs, TheDatabaseFileName);
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(KCreateTestTableSql);
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(KCreateIndexSql);
	TEST2(err, KErrNone);
	//	
	TBuf<KTextColSize> textColVal;
	for(TInt ii=23000;ii<(23000+KTextColSize);++ii)
		{
		textColVal.Append(TChar(ii));
		}
	err = TheDatabase.Begin();	
	TEST2(err, KErrNone);
	for(TInt i=0;i<KTestRecordCount;++i)
		{
		TBuf<KTextColSize + 100> sql;
		sql.Format(_L("INSERT INTO A(I1,I2,I3) VALUES(%d,%d,'%S')"), i, i + 100000, &textColVal);
		err = TheDatabase.Execute(sql);
		TEST2(err, 1);
		}
	err = TheDatabase.Commit();	
	TEST2(err, KErrNone);
	}

void DestroyDatabase()
	{
	TheRowSet.Close();
	TheDatabase.Close();
	TheDbs.Close();
	TInt err = TheFs.Delete(TheDatabaseFileName);
	TEST2(err, KErrNone);
	}

void GetFastCounterFrequency()
	{
	TheTest.Start(_L("Get fast counter frequency"));
	TEST2(HAL::Get(HAL::EFastCounterFrequency, TheCounterFreq), KErrNone);
	TheTest.Printf(_L("Counter frequency=%d\r\n"), TheCounterFreq);
	}
	
void FileBlockSizeTestsL()
	{
	TheTest.Next(_L("File Block Size test"));
	
	const TInt KBlockSizeLow = 64, KBlockSizeHigh = 1024 * 64;
	for(TInt blockSize=KBlockSizeLow;blockSize<=KBlockSizeHigh;blockSize*=2)
		{
		for(TInt addSize=-4;addSize<=4;addSize+=4)
			{
			TInt currBlockSize = blockSize + addSize;
			TBuf<100> title;
			title.Copy(_L("File block size "));
			title.AppendNum((TInt64)currBlockSize);
			title.Append(_L(" bytes"));
			TheTest.Printf(title);
			
			HBufC8* data = HBufC8::New(currBlockSize);
			TEST(data != NULL);
			TPtr8 dataPtr = data->Des();
			dataPtr.SetLength(currBlockSize);
			
			TInt err = TheTestFile.Replace(TheFs, TheTestFileName, EFileRead | EFileWrite);
			TEST2(err, KErrNone);
			err = TheTestFile.Write(dataPtr);
			TEST2(err, KErrNone);
			err = TheTestFile.Flush();
			TEST2(err, KErrNone);
			TUint32 fc = User::FastCounter();
			err = TheTestFile.Write(0, dataPtr);
			fc = CalcTickDiff(fc, User::FastCounter());
			PrintFcDiffAsUs(_L("###Time=%d us\r\n"), fc);
			TEST2(err, KErrNone);
			TheTestFile.Close();
			
			delete data;
			}
		}
	DeleteFile(TheTestFileName);
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-UT-3310
@SYMTestCaseDesc        DBMS, RDbView performance tests.
@SYMTestPriority        High
@SYMTestActions        	The test creates and fills a test table with integer, real and text column. Then
						executes 3 test subcases:
						- updates the integer column only in all records, using "SELECT I1,I2,I3" cursor;
						- updates the text column only in all records, using "SELECT I1,I2,I3" cursor;
						- updates all columns in all records, using "SELECT I1,I2,I3" cursor;
						The execution times are printed out.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ7141
*/
void RecordLenTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-3310 Record length test - create database"));
	CreateAndFillDatabase();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	TheTest.Printf(_L("Record length test - update the integer column only"));
	TUint32 fc = User::FastCounter();
	RDbView view;
	TInt err = view.Prepare(TheDatabase, _L("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = view.EvaluateAll();
	TEST2(err, KErrNone);
	TheRowSet = view;
	err = TheDatabase.Begin();
	TEST2(err, KErrNone);
	TInt recCnt = 0;
	TheRowSet.FirstL();
	do
		{
		TheRowSet.UpdateL();
		TheRowSet.SetColL(1, 60000 + recCnt);
		TheRowSet.PutL();
		++recCnt;
		}
	while(TheRowSet.NextL());
	TEST2(recCnt, KTestRecordCount);
	err = TheDatabase.Commit();
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###Time=%d us\r\n"), CalcTickDiff(fc, User::FastCounter()));
	TheRowSet.Close();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	TheTest.Printf(_L("Record length test - update the text column only"));
	fc = User::FastCounter();
	err = view.Prepare(TheDatabase, _L("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = view.EvaluateAll();
	TEST2(err, KErrNone);
	TheRowSet = view;
	err = TheDatabase.Begin();
	TEST2(err, KErrNone);
	recCnt = 0;
	TheRowSet.FirstL();
	do
		{
		TheRowSet.UpdateL();
		TheRowSet.SetColL(3, _L("0123456789"));
		TheRowSet.PutL();
		++recCnt;
		}
	while(TheRowSet.NextL());
	TEST2(recCnt, KTestRecordCount);
	err = TheDatabase.Commit();
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###Time=%d us\r\n"), CalcTickDiff(fc, User::FastCounter()));
	TheRowSet.Close();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	TheTest.Printf(_L("Record length test - update all columns"));
	fc = User::FastCounter();
	err = view.Prepare(TheDatabase, _L("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = view.EvaluateAll();
	TEST2(err, KErrNone);
	TheRowSet = view;
	err = TheDatabase.Begin();
	TEST2(err, KErrNone);
	recCnt = 0;
	TheRowSet.FirstL();
	do
		{
		TheRowSet.UpdateL();
		TheRowSet.SetColL(1, 34567 - recCnt);
		TheRowSet.SetColL(2, 888.111);
		TheRowSet.SetColL(3, _L("QWETYUIOPASDF"));
		TheRowSet.PutL();
		++recCnt;
		}
	while(TheRowSet.NextL());
	TEST2(recCnt, KTestRecordCount);
	err = TheDatabase.Commit();
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###Time=%d ms\r\n"), CalcTickDiff(fc, User::FastCounter()));
	////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	DestroyDatabase();//This will destroy TheRowSet object too.
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-UT-3311
@SYMTestCaseDesc        DBMS, RDbView performance tests.
@SYMTestPriority        High
@SYMTestActions        	The test measures the time used by insert-only or updatable cursor to:
						- insert certain amount of records, comitting after each insert;
						- insert certain amount of records, comitting after 20 inserts;
						- insert certain amount of records, comitting at the end;
						All cases repeated with and without compaction (except the first one).
						The execution times are printed out.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ7141
*/
void InsertTestsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-3311 RDbView \"insert\" performance tests")); 	
	for(TInt i1=0;i1<COUNT_OF(KRowSetTypes);++i1)
		{
		for(TInt i2=0;i2<COUNT_OF(KAccessModes);++i2)
			{
			for(TInt i3=0;i3<COUNT_OF(KCommitTypes);++i3)
				{
				CreateDatabase();
				TBuf<200> title;
				title.Copy(_L("Insert."));
				title.Append(KRowSetTypeStr[i1]);
				title.Append(KAccessModeStr[i2]);
				title.Append(KCommitTypeStr[i3]);
				TheTest.Printf(title);
				DbRowSetInsertTestL(KRowSetTypes[i1], KAccessModes[i2], KCommitTypes[i3]);
				DestroyDatabase();
				}
			}
		}
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-UT-3312
@SYMTestCaseDesc        DBMS, RDbView performance tests.
@SYMTestPriority        High
@SYMTestActions        	The test measures the time used by updatable cursor to:
						- update (moving forward/backward) certain amount of records, comitting after each update;
						- update (moving forward/backward) certain amount of records, comitting after 20 update;
						- update (moving forward/backward) certain amount of records, comitting at the end;
						All cases repeated with and without compaction (except the first one).
						The execution times are printed out.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ7141
*/
void UpdateTestsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-3312 RDbView \"update\" performance tests")); 	
	for(TInt i1=0;i1<COUNT_OF(KRowSetTypes);++i1)
		{
		for(TInt i2=0;i2<COUNT_OF(KUpdDirectionTypes);++i2)
			{
			for(TInt i3=0;i3<COUNT_OF(KCommitTypes);++i3)
				{
				CreateAndFillDatabase();
				TBuf<200> title;
				title.Copy(_L("Update."));
				title.Append(KRowSetTypeStr[i1]);
				title.Append(KUpdDirectionTypeStr[i2]);
				title.Append(KCommitTypeStr[i3]);
				TheTest.Printf(title);
				DbRowSetUpdateTestL(KRowSetTypes[i1], KUpdDirectionTypes[i2], KCommitTypes[i3]);
				DestroyDatabase();
				}
			}
		}
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-UT-3313
@SYMTestCaseDesc        DBMS, RDbDatabase::Execute() performance tests.
@SYMTestPriority        High
@SYMTestActions        	The test measures the time used by RDbDatabase::Execute() to:
						- insert certain amount of records, comitting after each insert;
						- insert certain amount of records, comitting after 20 inserts;
						- insert certain amount of records, comitting at the end;
						All cases repeated with and without compaction (except the first one).
						The execution times are printed out.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ7141
*/
void DbInsertTestsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-3313 RDbDatabase::Execute() \"insert\" performance tests")); 	
	TBuf<KTextColSize> textColVal;
	for(TInt ii=23000;ii<(23000+KTextColSize);++ii)
		{
		textColVal.Append(TChar(ii));
		}
	for(TInt compaction=0;compaction<2;++compaction)
		{
		for(TInt i3=0;i3<COUNT_OF(KCommitTypes);++i3)
			{
			CreateDatabase();
			TBuf<200> title;
			title.Copy(_L("RDbDatabase::Execute().Insert."));
			title.Append(KCommitTypeStr[i3]);
			title.Append(compaction ? _L("Compaction:Yes.") : _L("Compaction:No."));
			TheTest.Printf(title);
			TInt count = 0, err = KErrNone;
			TUint32 fc = User::FastCounter();
			for(TInt i=0;i<KTestRecordCount;++i)
				{
				if(KCommitTypes[i3] != ECommit_1_Rec && count == 0)
					{
					err = TheDatabase.Begin();	
					TEST2(err, KErrNone);
					}
				TBuf<KTextColSize + 100> sql;
				sql.Format(_L("INSERT INTO A(I1,I2,I3) VALUES(%d,%d,'%S')"), i, i + 100000, &textColVal);
				err = TheDatabase.Execute(sql);
				TEST2(err, 1);
				if(KCommitTypes[i3] != ECommit_1_Rec && ++count == KCommitTypes[i3])
					{
					count = 0;
					err = TheDatabase.Commit();	
					TEST2(err, KErrNone);
					if(compaction)
						{
						err = TheDatabase.Compact();
						TEST2(err, KErrNone);
						}
					}
				}
			PrintFcDiffAsUs(_L("###RDbDatabase::Execute(), Time=%d us\r\n"), CalcTickDiff(fc, User::FastCounter()));
			PrintFileSize();
			DestroyDatabase();
			}
		}
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-UT-3314
@SYMTestCaseDesc        DBMS, RDbDatabase::Execute() performance tests.
@SYMTestPriority        High
@SYMTestActions        	The test measures the time used by RDbDatabase::Execute() to:
						- update certain amount of records, comitting after each update;
						- update certain amount of records, comitting after 20 updates;
						- update certain amount of records, comitting at the end;
						All cases repeated with and without compaction (except the first one).
						The execution times are printed out.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ7141
*/
void DbUpdateTestsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-3314 RDbDatabase::Execute() \"update\" performance tests")); 	
	TBuf<KTextColSize> textColVal;
	for(TInt ii=43000;ii<(43000+KTextColSize);++ii)
		{
		textColVal.Append(TChar(ii));
		}
	for(TInt compaction=0;compaction<2;++compaction)
		{
		for(TInt i3=0;i3<COUNT_OF(KCommitTypes);++i3)
			{
			CreateAndFillDatabase();
			TBuf<200> title;
			title.Copy(_L("RDbDatabase::Execute().Update."));
			title.Append(KCommitTypeStr[i3]);
			title.Append(compaction ? _L("Compaction:Yes.") : _L("Compaction:No."));
			TheTest.Printf(title);
			TInt count = 0, err = KErrNone;
			TUint32 fc = User::FastCounter();
			for(TInt i=0;i<KTestRecordCount;++i)
				{
				if(KCommitTypes[i3] != ECommit_1_Rec && count == 0)
					{
					err = TheDatabase.Begin();	
					TEST2(err, KErrNone);
					}
				TBuf<KTextColSize + 100> sql;
				sql.Format(_L("UPDATE A SET I2=%d,I3='%S' WHERE I1=%d"), i + 556622, &textColVal, i);
				err = TheDatabase.Execute(sql);
				TEST2(err, 1);
				if(KCommitTypes[i3] != ECommit_1_Rec && ++count == KCommitTypes[i3])
					{
					count = 0;
					err = TheDatabase.Commit();	
					TEST2(err, KErrNone);
					if(compaction)
						{
						err = TheDatabase.Compact();
						TEST2(err, KErrNone);
						}
					}
				}
			PrintFcDiffAsUs(_L("###RDbDatabase::Execute(), Time=%d us\r\n"), CalcTickDiff(fc, User::FastCounter()));
			PrintFileSize();
			DestroyDatabase();
			}
		}
	}

/**
@SYMTestCaseID			PDS-DBMS-UT-4010
@SYMTestCaseDesc		Test for DEF141419 - DBMS, new performance tests - RDbUpdate, RDbStoreDatabase.
						Tests the performance of RDbColWriteStream::OpenLC() and RDbColWriteStream::OpenL(). 
@SYMTestPriority		High
@SYMTestActions			Test for DEF141419 - DBMS, new performance tests - RDbUpdate, RDbStoreDatabase
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF141419
*/
void DbColWriteStreamTestsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:PDS-DBMS-UT-4010 RDbColWriteStream performance test")); 	
	
	TInt err = TheDatabase.Replace(TheFs, TheDatabaseFileName);
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(KCreateTestTableSql2);
	TEST(err >= 0);

	RDbView view;
	err = view.Prepare(TheDatabase, _L("select * from A"), view.EInsertOnly);
	TEST2(err, KErrNone);
	TheRowSet = view;	

	CDbColSet* colSet = TheRowSet.ColSetL();
	const TDbColNo KIdx1 = colSet->ColNo(_L("I1"));
	const TDbColNo KIdx2 = colSet->ColNo(_L("I2"));
	const TDbColNo KIdx3 = colSet->ColNo(_L("I3"));
	delete colSet;
	
	TheTextRndData.SetLength(KColSize2);
	TheTextRndData.Fill(KBufChar);
	TheBinRndData.SetLength(KColSize2);
	TheBinRndData.Fill(KBufChar);

	err = TheDatabase.Begin();
	TEST2(err, KErrNone);
	
	TUint32 openLcFc = 0;
	TUint32 openLFc = 0;
	TUint32 fc = User::FastCounter();
	
	for(TInt i=0;i<KTestRecordCount;++i)
		{
		TheRowSet.InsertL();
		TheRowSet.SetColL(KIdx1, i + 1);
		
		RDbColWriteStream strm1;
		TUint32 tmp = User::FastCounter();
		strm1.OpenLC(TheRowSet, KIdx2);
		openLcFc += CalcTickDiff(tmp, User::FastCounter());
		strm1.WriteL(TheTextRndData, KColSize2);
		strm1.CommitL();
		CleanupStack::PopAndDestroy(&strm1);

		RDbColWriteStream strm2;
		CleanupClosePushL(strm2);
		tmp = User::FastCounter();
		strm2.OpenL(TheRowSet, KIdx3);
		openLFc += CalcTickDiff(tmp, User::FastCounter());
		strm2.WriteL(TheBinRndData, KColSize2);
		strm2.CommitL();
		CleanupStack::PopAndDestroy(&strm2);
		
		TheRowSet.PutL();
		}
	
	PrintFcDiffAsUs2(_L("###RDbColWriteStream, write %3d records, Time=%d us\r\n"), 
										KTestRecordCount, CalcTickDiff(fc, User::FastCounter()));
	PrintFcDiffAsUs (_L("###OpenLC()                            , Time=%d us\r\n"), openLcFc);
	PrintFcDiffAsUs (_L("###OpenL()                             , Time=%d us\r\n"), openLFc);
	
	TheRowSet.Close();
	
	err = TheDatabase.Commit();
	TEST2(err, KErrNone);
	
	TheDatabase.Close();
	}

/**
@SYMTestCaseID			PDS-DBMS-UT-4011
@SYMTestCaseDesc		Test for DEF141419 - DBMS, new performance tests - RDbUpdate, RDbStoreDatabase.
						Tests the performance of RDbColReadStream::OpenLC() and RDbColReadStream::OpenL(). 
@SYMTestPriority		High
@SYMTestActions			Test for DEF141419 - DBMS, new performance tests - RDbUpdate, RDbStoreDatabase.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF141419
*/
void DbColReadStreamTestsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:PDS-DBMS-UT-4011 RDbColReadStream performance test")); 	
	
	TInt err = TheDatabase.Open(TheFs, TheDatabaseFileName);
	TEST2(err, KErrNone);

	RDbView view;
	err = view.Prepare(TheDatabase, _L("select * from A"), view.EReadOnly);
	TEST2(err, KErrNone);
	TheRowSet = view;	

	err = view.EvaluateAll();
	TEST2(err, KErrNone);
	
	CDbColSet* colSet = TheRowSet.ColSetL();
	const TDbColNo KIdx1 = colSet->ColNo(_L("I1"));
	const TDbColNo KIdx2 = colSet->ColNo(_L("I2"));
	const TDbColNo KIdx3 = colSet->ColNo(_L("I3"));
	delete colSet;

	TBuf8<KColSize2> binData;
	TBuf16<KColSize2> textData;
	
	TInt recCnt = 0;
	TUint32 openLcFc = 0;
	TUint32 openLFc = 0;
	TUint32 fc = User::FastCounter();

	if(TheRowSet.FirstL())
		{
		do
			{
			TheRowSet.GetL();
			++recCnt;
			
			TInt i1 = TheRowSet.ColInt(KIdx1);
			TEST2(recCnt, i1);
			
			RDbColReadStream strm1;
			TUint32 tmp = User::FastCounter();
			strm1.OpenLC(TheRowSet, KIdx2);
			openLcFc += CalcTickDiff(tmp, User::FastCounter());
			strm1.ReadL(textData, KColSize2);
			CleanupStack::PopAndDestroy(&strm1);
			TEST(textData == TheTextRndData);
				
			RDbColReadStream strm2;
			CleanupClosePushL(strm2);
			tmp = User::FastCounter();
			strm2.OpenL(TheRowSet, KIdx3);
			openLFc += CalcTickDiff(tmp, User::FastCounter());
			strm2.ReadL(binData, KColSize2);
			CleanupStack::PopAndDestroy(&strm2);
			TEST(binData == TheBinRndData);
			} while(TheRowSet.NextL());
		}
	TEST2(recCnt, KTestRecordCount);
	
	PrintFcDiffAsUs2(_L("###RDbColReadStream, read %3d records, Time=%d us\r\n"), 
			                                 recCnt, CalcTickDiff(fc, User::FastCounter()));
	PrintFcDiffAsUs (_L("###OpenLC()                          , Time=%d us\r\n"), openLcFc);
	PrintFcDiffAsUs (_L("###OpenL()                           , Time=%d us\r\n"), openLFc);
	
	TheRowSet.Close();
	
	TheDatabase.Close();
	}

/**
@SYMTestCaseID			PDS-DBMS-UT-4012
@SYMTestCaseDesc		Test for DEF141419 - DBMS, new performance tests - RDbUpdate, RDbStoreDatabase.
						RDbUpdate performance tests.
@SYMTestPriority		High
@SYMTestActions			Test for DEF141419 - DBMS, new performance tests - RDbUpdate, RDbStoreDatabase
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF141419
*/
void DbUpdateTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:PDS-DBMS-UT-4012 RDbUpdate performance test")); 	
	
	TInt err = TheDbs.Connect();
	TEST2(err, KErrNone);
		
	err = TheDatabase.Open(TheDbs, TheDatabaseFileName);
	TEST2(err, KErrNone);
	
	TUint32 execFc = 0;
	TUint32 nextFc = 0;
	TUint32 rowCntFc = 0;
	
	//Synchronous update
	TInt recCnt = 0;
	RDbUpdate upd1;
	TUint32 fc = User::FastCounter();
	err = upd1.Execute(TheDatabase, _L("UPDATE A SET I1=1000, I2='8888888888888888'"));
	execFc = CalcTickDiff(fc, User::FastCounter());
	TEST2(err, KErrNone);
	TUint32 tmp = User::FastCounter();
	while((err = upd1.Next()) > 0)
		{
		}
	nextFc = CalcTickDiff(tmp, User::FastCounter());
	tmp = User::FastCounter();
	recCnt = upd1.RowCount();
	rowCntFc = CalcTickDiff(tmp, User::FastCounter());
	upd1.Close();
	fc = CalcTickDiff(fc, User::FastCounter());
	TEST2(err, 0);
	TEST2(recCnt, KTestRecordCount);
	
	TheDatabase.Close();

	PrintFcDiffAsUs2(_L("###Sync RDbUpdate, %3d records, Time=%d us\r\n"), recCnt, fc);
	PrintFcDiffAsUs (_L("###Execute()                  , Time=%d us\r\n"), execFc);
	PrintFcDiffAsUs (_L("###Next()                     , Time=%d us\r\n"), nextFc);
	PrintFcDiffAsUs (_L("###RowCount()                 , Time=%d us\r\n"), rowCntFc);
	
	//Asynchronous update
	err = TheDatabase.Open(TheDbs, TheDatabaseFileName);
	TEST2(err, KErrNone);
	
	execFc = 0;
	nextFc = 0;
	rowCntFc = 0;
	recCnt = 0;
	
	RDbUpdate upd2;
	fc = User::FastCounter();
	err = upd2.Execute(TheDatabase, _L("UPDATE A SET I1=5678, I2='9w27u22272252542r2242242424221'"));
	execFc = CalcTickDiff(fc, User::FastCounter());
	TEST2(err, KErrNone);
	TRequestStatus stat;
	tmp = User::FastCounter();
	do
		{
		upd2.Next(stat);
		User::WaitForRequest(stat);
		} while(stat.Int() > 0);
	nextFc = CalcTickDiff(tmp, User::FastCounter());
	tmp = User::FastCounter();
	recCnt = upd2.RowCount();
	rowCntFc = CalcTickDiff(tmp, User::FastCounter());
	upd2.Close();
	fc = CalcTickDiff(fc, User::FastCounter());
	TEST2(stat.Int(), 0);
	TEST2(recCnt, KTestRecordCount);

	TheDatabase.Close();
	
	PrintFcDiffAsUs2(_L("###Async RDbUpdate, %3d records, Time=%d us\r\n"), recCnt, fc);
	PrintFcDiffAsUs (_L("###Execute()                   , Time=%d us\r\n"), execFc);
	PrintFcDiffAsUs (_L("###Next()                      , Time=%d us\r\n"), nextFc);
	PrintFcDiffAsUs (_L("###RowCount()                  , Time=%d us\r\n"), rowCntFc);
	
	TheDbs.Close();
	err = TheFs.Delete(TheDatabaseFileName);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			PDS-DBMS-UT-4013
@SYMTestCaseDesc		Test for DEF141419 - DBMS, new performance tests - RDbUpdate, RDbStoreDatabase.
						RDbStoreDatabase performance tests.
@SYMTestPriority		High
@SYMTestActions			Test for DEF141419 - DBMS, new performance tests - RDbUpdate, RDbStoreDatabase.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF141419
*/
void DbStoreDatabaseTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:PDS-DBMS-UT-4013 RDbStoreDatabase performance test")); 	
	
	CFileStore* fstore = CPermanentFileStore::ReplaceLC(TheFs, TheDatabaseFileName, EFileRead | EFileWrite);
	fstore->SetTypeL(fstore->Layout());
	
	//Create the database, insert records, compress the store.
	
	RDbStoreDatabase db1;
	CleanupClosePushL(db1);
	TUint32 fc = User::FastCounter();
	TStreamId strmId = db1.CreateL(fstore);
	TUint32 createFc = CalcTickDiff(fc, User::FastCounter());
	
	TInt err = db1.Execute(KCreateTestTableSql2);
	TEST(err >= 0);
	err = db1.Begin();
	TEST2(err, 0);
	for(TInt i=0;i<KTestRecordCount;++i)
		{
		err = db1.Execute(_L("INSERT INTO A(I1, I2) VALUES(1, 'ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZAAAAAAAAAAAAAAAAAAAAAAAAAAAA')"));
		TEST2(err, 1);
		}
	err = db1.Commit();
	TEST2(err, 0);

	CleanupStack::PopAndDestroy(&db1);
	TUint32 writeFc = CalcTickDiff(fc, User::FastCounter());
	
	TInt fsize1 = 0;
	err = fstore->File().Size(fsize1);
	TEST2(err, KErrNone);
	
	fc = User::FastCounter();
	RDbStoreDatabase::CompressL(*fstore, strmId);
	TUint32 compressFc = CalcTickDiff(fc, User::FastCounter());

	TInt fsize2 = 0;
	err = fstore->File().Size(fsize2);
	TEST2(err, KErrNone);
	
	CleanupStack::PopAndDestroy(fstore);
	
	PrintFcDiffAsUs2(_L("###RDbStoreDatabase, write %3d records, Time=%d us\r\n"), KTestRecordCount, writeFc);
	PrintFcDiffAsUs (_L("###CreateL()                          , Time=%d us\r\n"), createFc);
	PrintFcDiffAsUs (_L("###CompressL()                        , Time=%d us\r\n"), compressFc);
	
	//Decompress the store, open the database, read the records

	fstore = CPermanentFileStore::OpenLC(TheFs, TheDatabaseFileName, EFileRead | EFileWrite);
	fstore->SetTypeL(fstore->Layout());
	
	fc = User::FastCounter();
	RDbStoreDatabase::DecompressL(*fstore, strmId);
	TUint32 decompressFc = CalcTickDiff(fc, User::FastCounter());

	TInt fsize3 = 0;
	err = fstore->File().Size(fsize3);
	TEST2(err, KErrNone);
	
	RDbStoreDatabase db2;
	CleanupClosePushL(db2);
	fc = User::FastCounter();
	db2.OpenL(fstore, strmId);
	TUint32 openFc = CalcTickDiff(fc, User::FastCounter());
	
	RDbView view;
	err = view.Prepare(db2, _L("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = view.EvaluateAll();
	TEST2(err, KErrNone);	
	TheRowSet = view;
	
	CDbColSet* colSet = TheRowSet.ColSetL();
	const TDbColNo KIdx1 = colSet->ColNo(_L("I1"));
	const TDbColNo KIdx2 = colSet->ColNo(_L("I2"));
	const TDbColNo KIdx3 = colSet->ColNo(_L("I3"));
	delete colSet;

	TInt recCnt = 0;
	if(TheRowSet.FirstL())
		{
		do
			{
			TheRowSet.GetL();
			++recCnt;
			
			RDbColReadStream strm1;
			strm1.OpenLC(TheRowSet, KIdx2);
			TBuf<KColSize2> buf;
			_LIT(KText, "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZAAAAAAAAAAAAAAAAAAAAAAAAAAAA");//the same as the used in the INSERT statement above
			strm1.ReadL(buf, KText().Length());
			CleanupStack::PopAndDestroy(&strm1);
			TEST(KText() == buf);
			} while(TheRowSet.NextL());
		}
	TEST2(recCnt, KTestRecordCount);
	
	TheRowSet.Close();
	TUint32 readFc = CalcTickDiff(fc, User::FastCounter());
	
	CleanupStack::PopAndDestroy(&db2);
	CleanupStack::PopAndDestroy(fstore);
	
	PrintFcDiffAsUs2(_L("###RDbStoreDatabase, read %3d records , Time=%d us\r\n"), recCnt, readFc);
	PrintFcDiffAsUs (_L("###OpenL()                            , Time=%d us\r\n"), openFc);
	PrintFcDiffAsUs (_L("###DecompressL()                      , Time=%d us\r\n"), decompressFc);
	TheTest.Printf(_L("###File size. After write: %d. After Compress(): %d. After Decompress(): %d\r\n"), fsize1, fsize2, fsize3);
	
	err = TheFs.Delete(TheDatabaseFileName);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			PDS-DBMS-UT-4014
@SYMTestCaseDesc		Test for DEF141419 - DBMS, new performance tests - RDbUpdate, RDbStoreDatabase.
						RDbDatabase performance tests.
@SYMTestPriority		High
@SYMTestActions			Test for DEF141419 - DBMS, new performance tests - RDbUpdate, RDbStoreDatabase.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF141419
*/
void DbDatabaseTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:PDS-DBMS-UT-4014 RDbDatabase performance test")); 	
	
	TInt err = TheDatabase.Replace(TheFs, TheDatabaseFileName);
	TEST2(err, KErrNone);
	
	//CDbColSet
	_LIT(KColName1, "Col1");
	_LIT(KColName2, "Col2");
	_LIT(KColName3, "Col3");
	TUint32 fc = User::FastCounter();
	CDbColSet* colset = CDbColSet::NewLC();
	TDbCol col1(KColName1, EDbColInt32);
	colset->AddL(col1);
	TDbCol col2(KColName2, EDbColText16, 100);
	colset->AddL(col2);
	TDbCol col3(KColName3, EDbColBinary, 100);
	colset->AddL(col3);
	TUint32 colSetFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###CDbColSet::NewLC() & CDbColSet::AddL(), Time=%d us\r\n"), colSetFc);
	
	//RDbDatabase::CreateTable()
	_LIT(KTblName, "ATbl");
	fc = User::FastCounter();
	err = TheDatabase.CreateTable(KTblName, *colset);
	TUint32 createTblFc = CalcTickDiff(fc, User::FastCounter());
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###RDbDatabase::CreateTable(), Time=%d us\r\n"), createTblFc);
	
	//RDbDatabase::AlterTable()
	_LIT(KColName4, "Col4");
	TDbCol col4(KColName4, EDbColReal64);
	colset->AddL(col4);
	fc = User::FastCounter();
	err = TheDatabase.AlterTable(KTblName, *colset);
	TUint32 alterTblFc = CalcTickDiff(fc, User::FastCounter());
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###RDbDatabase::AlterTable(), Time=%d us\r\n"), alterTblFc);

	//CDbKey
	fc = User::FastCounter();
	CDbKey* dbKey = CDbKey::NewLC();
	TDbKeyCol keyCol1(KColName1, TDbKeyCol::EAsc);
	dbKey->AddL(keyCol1);
	TDbKeyCol keyCol2(KColName4, TDbKeyCol::EDesc);
	dbKey->AddL(keyCol2);
	TUint32 dbKeyFc = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L("###CDbKey::NewLC() & CDbKey::AddL(), Time=%d us\r\n"), dbKeyFc);
	
	//RDbDatabase::CreateIndex()
	_LIT(KKeyName, "AKey");
	fc = User::FastCounter();
	err = TheDatabase.CreateIndex(KKeyName, KTblName, *dbKey);
	TUint32 createIndexFc = CalcTickDiff(fc, User::FastCounter());
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###RDbDatabase::CreateIndex(), Time=%d us\r\n"), createIndexFc);
	
	CleanupStack::PopAndDestroy(dbKey);
	CleanupStack::PopAndDestroy(colset);
	
	//RDbDatabase::TableNamesL()
	fc = User::FastCounter();
	CDbTableNames* tblNames = TheDatabase.TableNamesL();
	TUint32 tblNamesFc = CalcTickDiff(fc, User::FastCounter());
	delete tblNames;
	PrintFcDiffAsUs(_L("###RDbDatabase::TableNamesL(), Time=%d us\r\n"), tblNamesFc);
	
	//RDbDatabase::IndexNamesL()
	fc = User::FastCounter();
	CDbIndexNames* idxNames = TheDatabase.IndexNamesL(KTblName);
	TUint32 idxNamesFc = CalcTickDiff(fc, User::FastCounter());
	delete idxNames;
	PrintFcDiffAsUs(_L("###RDbDatabase::IndexNamesL(), Time=%d us\r\n"), idxNamesFc);

	//RDbDatabase::ColSetL()
	fc = User::FastCounter();
	colset = TheDatabase.ColSetL(KTblName);
	colSetFc = CalcTickDiff(fc, User::FastCounter());
	delete colset;
	PrintFcDiffAsUs(_L("###RDbDatabase::ColSetL(), Time=%d us\r\n"), colSetFc);
	
	//RDbDatabase::KeyL()
	fc = User::FastCounter();
	dbKey =  TheDatabase.KeyL(KKeyName, KTblName);
	dbKeyFc = CalcTickDiff(fc, User::FastCounter());
	delete dbKey;
	PrintFcDiffAsUs(_L("###RDbDatabase::KeyL(), Time=%d us\r\n"), dbKeyFc);

	//RDbDatabase::DropIndex()
	fc = User::FastCounter();
	err = TheDatabase.DropIndex(KKeyName, KTblName);
	TUint32 dropIdxFc = CalcTickDiff(fc, User::FastCounter());
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###RDbDatabase::DropIndex(), Time=%d us\r\n"), dropIdxFc);
	
	//RDbDatabase::DropTable()
	fc = User::FastCounter();
	err = TheDatabase.DropTable(KTblName);
	TUint32 dropTblFc = CalcTickDiff(fc, User::FastCounter());
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###RDbDatabase::DropTable(), Time=%d us\r\n"), dropTblFc);
	
	TheDatabase.Close();
	}

void PrintDiskUsage(const TDesC& aPath, TInt aOffset = 0)
	{
	_LIT(KSpace, " ");
	TheTest.Printf(_L("%*.*S%S\r\n"), aOffset, aOffset, &KSpace, &aPath);
	TFindFile* findFile = new TFindFile(TheFs);//TFindFile has TParse data member. 
											   //Since this function is called recoursively, on some platforms
											   //the test might crash - "stack overflow" problem.
	TEST(findFile != NULL);
	CDir* fileNameCol = NULL;
	TBuf<8> fileNameMask;
	fileNameMask.Copy(_L("*.*"));
	TInt err = findFile->FindWildByDir(fileNameMask, aPath, fileNameCol);
	if(err == KErrNone)
		{
		do
			{
			const TDesC& file = findFile->File();//"file" variable contains the drive and the path. the file name in "file" is invalid in this case.
			(void)TheParse.Set(file, NULL, NULL);
			TPtrC driveName = TheParse.Drive();
			if(aPath.FindF(driveName) >= 0)
				{		
                TInt cnt = fileNameCol->Count();
                for(TInt i=0;i<cnt;++i)
                    {
                    const ::TEntry& entry = (*fileNameCol)[i];
                    if(!entry.IsDir())
                        {
                        TheTest.Printf(_L("%*.*S    %S, size=%d\r\n"), aOffset, aOffset, &KSpace, &entry.iName, entry.iSize);
                        }
                    else
                        {
						TFileName* path = new TFileName;//allocated in heap to prevent "stack overflow" prolem
						TEST(path != NULL);
						path->Copy(aPath);
						path->Append(entry.iName);
						path->Append(_L("\\"));
                        PrintDiskUsage(*path, aOffset + 4);
                        delete path;
                        }
                    }
				} // if(aPath.FindF(driveName) >= 0)
			
			delete fileNameCol;
			fileNameCol = NULL;
			} while((err = findFile->FindWild(fileNameCol)) == KErrNone);//Get the next set of files
		}
	else
		{
		TheTest.Printf(_L("  FindWildByDir() failed with err=%d\r\n"), err);
		}
	delete findFile;
	}

void DoTestL()
	{
	GetFastCounterFrequency();

	FileBlockSizeTestsL();

	RecordLenTestL();

	InsertTestsL();

	UpdateTestsL();

	DbInsertTestsL();

	DbUpdateTestsL();

	DbColWriteStreamTestsL();

	DbColReadStreamTestsL();

	DbUpdateTestL();

	DbStoreDatabaseTestL();

	DbDatabaseTestL();
	}

//Usage: "t_dbperf2 [<drive letter>:]"
TInt E32Main()
    {
	TheTest.Title();
	
	TheTrapCleanup = CTrapCleanup::New();
	TEST(TheTrapCleanup != NULL);
	
	//Construct test database file name
	_LIT(KTestDatabase, "c:\\dbms-tst\\t_dbperf2.db");
	TFileName fname;
	User::CommandLine(fname);
	TParse parse;
	parse.Set(fname, &KTestDatabase, 0);
	const TDesC& dbFilePath = parse.FullName();
	TheDatabaseFileName.Copy(dbFilePath);
	TheTest.Printf(_L("Test database: %S\r\n"), &TheDatabaseFileName);
	//Construct test file name
	_LIT(KTestFile, "c:\\dbms-tst\\t_dbperf2.dat");
	parse.Set(fname, &KTestFile, 0);
	const TDesC& testFilePath = parse.FullName();
	TheTestFileName.Copy(testFilePath);

	__UHEAP_MARK;

	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);
	err = TheFs.MkDir(TheDatabaseFileName);
	TheTest.Printf(_L("MkDir(): err=%d\r\n"), err);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	DeleteFile(TheDatabaseFileName);
	TRAP(err, DoTestL());
	DeleteFile(TheDatabaseFileName);
	DeleteFile(TheTestFileName);
	TEST2(err, KErrNone);

	TheTest.Printf(_L("====================== Disk usage ==================\r\n"));
	PrintDiskUsage(_L("c:\\"));
	TheTest.Printf(_L("====================================================\r\n"));
	
	CloseAll();

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();
	
	delete TheTrapCleanup;
	return KErrNone;
    }
