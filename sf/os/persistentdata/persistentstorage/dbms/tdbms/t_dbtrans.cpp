// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <hal.h>

static RTest TheTest(_L("t_dbtrans: Test DBMS transactions"));
static RDbTable TheTable;
static RFs TheFs;
static RDbs TheDbs;
static RDbNamedDatabase TheDatabase;

static TFileName TheTestDbFileName;

const TPtrC KTableName(_S("table"));
const TPtrC KIndexInt=_S("int");
const TPtrC KIndexText=_S("text");
const TPtrC KColumnInt=_S("int");
const TPtrC KColumnText=_S("text");
const TPtrC KColumnComment=_S("comment");
const TPtrC KCommentValue=_S("abcdefghijklmnopqrstuvwxyz");
const TInt KRecords=2000;

///////////////////////////////////////////////////////////////////////////////////////

static void CloseAll()
	{
	TheTable.Close();
	TheDatabase.Close();
	TheDbs.Close();
	}

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

static void DestroyTestEnv()
	{
	CloseAll();
	DeleteFile(TheTestDbFileName);
	TheFs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
//Tests macros and functions.
//If (!aValue) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest.Printf(_L("*** Boolean expression evaluated to false!\r\n"));
		DestroyTestEnv();
		TheTest(EFalse, aLine);
		}
	}
//If (aValue != aExpected) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TheTest.Printf(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		DestroyTestEnv();
		TheTest(EFalse, aLine);
		}
	}
//Use these to test conditions.
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

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
	if(TheCounterFreq <= 0)
		{
		TEST2(HAL::Get(HAL::EFastCounterFrequency, TheCounterFreq), KErrNone);
		TheTest.Printf(_L("Counter frequency=%d Hz\r\n"), TheCounterFreq);
		}
	double v = ((double)aFastCount * KMicroSecIn1Sec) / (double)TheCounterFreq;
	TInt v2 = (TInt)v;
	TheTest.Printf(aFormatStr, v2);
	}

///////////////////////////////////////////////////////////////////////////////////////

//
// Prepare the test directory.
//
void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TheTest(err == KErrNone);

	err = TheFs.MkDirAll(TheTestDbFileName);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

///////////////////////////////////////////////////////////////////////////////////////

class Progress
	{
	enum {ETotal=32};
public:
	Progress(TInt aCount);
	void Next(TInt aStep);
private:
	TInt iCount;
	TInt iPos;
	};

Progress::Progress(TInt aCount)
	: iCount(aCount),iPos(0)
	{}

void Progress::Next(TInt aStep)
	{
	TInt next=(ETotal*(iCount-aStep))/iCount;
	if (next!=iPos)
		{
		iPos=next;
		}
	}

static void ProgressInc(RDbIncremental& inc,TInt aCount)
	{
	Progress progress(aCount);
	while (aCount)
		{
		inc.Next(aCount);
		progress.Next(aCount);
		}
	inc.Close();
	}

//
// Create the database
//
static void CreateDatabase()
	{
	TInt err = TheDatabase.Replace(TheFs, TheTestDbFileName);
	TEST2(err, KErrNone);
	}

//
// Create the database
//
static void OpenDatabase()
	{
	TInt err = TheDatabase.Open(TheFs, TheTestDbFileName);
	TEST2(err, KErrNone);
	}

static void CloseDatabase()
	{
	TheDatabase.Close();
	}

static void CreateTable()
	{
	TInt err = TheDatabase.Execute(_L("create table table (int integer,text varchar(8),comment varchar)"));
	TEST2(err, KErrNone);
	}

static void WriteRecordsL(TInt aCount)
	{
	Progress write(aCount);
	TDbColNo cInt,cText,cComment;
	CDbColSet* set=TheTable.ColSetL();
	cInt=set->ColNo(KColumnInt);
	cText=set->ColNo(KColumnText);
	cComment=set->ColNo(KColumnComment);
	delete set;
	TBuf<10> text;
	TInt jj=0;
	for (TInt ii=0;ii<aCount;++ii)
		{
		TheTable.InsertL();
		jj=(jj+23);
		if (jj>=aCount)
			jj-=aCount;
		TheTable.SetColL(cInt,jj);
		text.Num(jj);
		TheTable.SetColL(cText,text);
		TheTable.SetColL(cComment,KCommentValue);
		TheTable.PutL();
		write.Next(aCount-ii-1);
		}
	}

static TUint FileSize()
	{
	TEntry entry;
	TEST2(TheFs.Entry(TheTestDbFileName, entry), KErrNone);
	return entry.iSize;
	}

static void BuildTableL(TInt aCount, TBool aTransactions, TUint32& aTicks, TUint& aSize)
	{
	TUint size = FileSize();
	TUint fc = User::FastCounter();
	CreateTable();
	if(aTransactions)
		{
		TheDatabase.Begin();
		}
	TEST2(TheTable.Open(TheDatabase, KTableName), KErrNone);
	WriteRecordsL(aCount);
	if(aTransactions)
		{
		TEST2(TheDatabase.Commit(), KErrNone);
		}
	TheTable.Close();
	aTicks = CalcTickDiff(fc, User::FastCounter());
	aSize = FileSize() - size;
	}

static void Execute(const TDesC& aSql)
	{
	RDbIncremental inc;
	TInt step;
	TEST2(inc.Execute(TheDatabase,aSql,step), KErrNone);
	ProgressInc(inc,step);
	}

static void BreakIndexL()
	{
	TheDatabase.Begin();
	TEST2(TheTable.Open(TheDatabase,KTableName), KErrNone);
	TheTable.InsertL();
	TheTable.SetColL(1,-1);
	TheTable.PutL();
	TheTable.Close();
	TheDatabase.Rollback();
	TEST(TheDatabase.IsDamaged());
	}

LOCAL_C void Recover()
	{
	RDbIncremental rec;
	TInt step;
	TEST2(rec.Recover(TheDatabase,step), KErrNone);
	ProgressInc(rec,step);
	TEST(!TheDatabase.IsDamaged());
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0637
@SYMTestCaseDesc        Streaming conversions test
@SYMTestPriority        Medium
@SYMTestActions         Test the database definition and enquiry functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void TestL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0637 Build without transactions "));
	CreateDatabase();
	TUint32 ticks1;
	TUint size1;
	BuildTableL(KRecords, EFalse, ticks1, size1);
	CloseDatabase();
	
	TheTest.Next(_L("Build with transactions"));
	CreateDatabase();
	TUint32 ticks2;
	TUint size2;
	BuildTableL(KRecords, ETrue, ticks2, size2);
	
	PrintFcDiffAsUs(_L("#### Without transactions, time=%d us\n"), ticks1);
	PrintFcDiffAsUs(_L("#### With    transactions, time=%d us\n"), ticks2);
	TheTest.Printf(_L("Transaction performance ratio (without trn:with trn): time %4.2f, size %4.2f\n"), TReal(ticks1) / TReal(ticks2), TReal(size1) / TReal(size2));
	
	TheTest.Next(_L("Build Int index"));
	Execute(_L("create unique index int on table (int)"));
	
	TheTest.Next(_L("Break index"));
	BreakIndexL();
	
	TheTest.Next(_L("Build Text index"));
	Execute(_L("create unique index text on table (text)"));
	
	TheTest.Next(_L("Recover"));
	TEST(TheDatabase.IsDamaged());
	
	CloseDatabase();
	OpenDatabase();
	TEST(TheDatabase.IsDamaged());
	Recover();
	
	TheTest.Next(_L("Drop table"));
	Execute(_L("drop table table"));
	CloseDatabase();
	}

//Usage: "t_trans [<drive letter>:]]"
TInt E32Main()
    {
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	TBuf<256> cmdline;
	User::CommandLine(cmdline);

	TParse parse;

	_LIT(KTestDatabase, "C:\\DBMS-TST\\T_TRANS.DB");
	parse.Set(cmdline, &KTestDatabase, 0);
	TheTestDbFileName.Copy(parse.FullName());
	
	__UHEAP_MARK;
	
	CreateTestEnv();
	TRAPD(err, TestL());
	TEST2(err, KErrNone);
	DestroyTestEnv();

	delete tc;

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	return 0;
    }
