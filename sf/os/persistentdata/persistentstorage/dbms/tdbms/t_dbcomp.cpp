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

// MSVC++ up to 5.0 has problems with expanding inline functions
// This disables the mad warnings for the whole project
#if defined(NDEBUG) && defined(__VC32__) && _MSC_VER<=1100
#pragma warning(disable : 4710)			// function not expanded. MSVC 5.0 is stupid
#endif

#include <d32dbms.h>
#include <s32file.h>
#include <e32test.h>
#include <e32math.h>
#include <hal.h>

static RTest TheTest(_L("t_dbcomp"));
static RFs   TheFs;

//T_BENCH.DB file is created by T_BENCH test and is used by the current test (T_COMP).
//T_COMP test will delete T_BENCH.DB at the end as it is no more needed.
//If you want to rerun T_COMP test again, you have to ensure that T_BENCH.DB file exists -
//run T_BENCH test again.
TFileName TheBenchDbFileName;
TFileName TheCompressedFileName;
TFileName TheDecompressedFileName;

RDbNamedDatabase TheCompDb;
RDbNamedDatabase TheCopyDb;
RDbTable 		 TheTable1, TheTable2;

///////////////////////////////////////////////////////////////////////////////////////

static void CloseAll()
	{
	TheTable2.Close();
	TheTable1.Close();
	TheCopyDb.Close();
	TheCompDb.Close();
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
	DeleteFile(TheDecompressedFileName);
	DeleteFile(TheCompressedFileName);
	DeleteFile(TheBenchDbFileName);
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

	err = TheFs.MkDirAll(TheBenchDbFileName);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	TUint dummy;
	err = TheFs.Att(TheBenchDbFileName, dummy);
	if(err != KErrNone)
		{
		TheTest.Printf(_L("*** The %S file does not exist, err=%d. Run \"T_BENCH\" test first!\r\n"), &TheBenchDbFileName, err);
		TEST2(err, KErrNone);
		}
	}

///////////////////////////////////////////////////////////////////////////////////////

//
// Compress the database
//
void CompressL(const TDesC& aSource, const TDesC& aTarget, TBool aCompress)
	{
	CFileMan* man = CFileMan::NewL(TheFs);
	TInt err = man->Copy(aSource, aTarget);
	delete man;
	User::LeaveIfError(err);
	
	User::LeaveIfError(TheFs.SetAtt(aTarget, 0, KEntryAttReadOnly));
	CFileStore* store = CFileStore::OpenLC(TheFs, aTarget, EFileRead|EFileWrite);
	
	TUint32 fc = User::FastCounter();
	if(aCompress)
		{
		RDbStoreDatabase::CompressL(*store, store->Root());
		}
	else
		{
		RDbStoreDatabase::DecompressL(*store, store->Root());
		}
	TUint32 time = CalcTickDiff(fc, User::FastCounter());
	PrintFcDiffAsUs(_L(" %d us\r\n"), time);
	
	store->CompactL();
	store->CommitL();
	
	CleanupStack::PopAndDestroy(store);
	}

void CheckTableL(RDbDatabase& aDatabase, RDbDatabase& aCopy, const TDesC& aTable)
	{
	TheTest.Printf(_L("Processing table %S\n"), &aTable);
	
	TInt err = TheTable1.Open(aDatabase, aTable, RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	
	err = TheTable2.Open(aCopy, aTable, RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	
	TInt columns = TheTable1.ColCount();
	while(TheTable1.NextL())
		{
		TheTable1.GetL();
		TEST(TheTable2.NextL());
		TheTable2.GetL();
		for(TInt ii=1;ii<=columns;++ii)
			{
			if(TDbCol::IsLong(TheTable1.ColType(ii)))
				{
				TInt len = TheTable1.ColSize(ii);
				TEST2(len, TheTable2.ColSize(ii));
				RDbColReadStream strm1;
				strm1.OpenLC(TheTable1, ii);
				RDbColReadStream strm2;
				strm2.OpenLC(TheTable2, ii);
				TBuf8<512> buf1;
				TBuf8<512> buf2;
				while(len)
					{
					TInt block = Min(512, len);
					strm1.ReadL(buf1, block);
					strm2.ReadL(buf2, block);
					TEST(buf1 == buf2);
					len -= block;
					}
				CleanupStack::PopAndDestroy(2);
				}
			else
				{
				TEST(TheTable1.ColDes8(ii) == TheTable2.ColDes8(ii));
				}
			}
		}
	TheTable2.Close();
	TheTable1.Close();
	}

void CheckL(const TDesC& aSource, const TDesC& aTarget)
	{
	TInt err = TheCompDb.Open(TheFs, aSource, KNullDesC, RDbNamedDatabase::EReadOnly);
	TEST2(err, KErrNone);
	
	err = TheCopyDb.Open(TheFs, aTarget, KNullDesC, RDbNamedDatabase::EReadOnly);
	TEST2(err, KErrNone);
	
	CDbTableNames* tables = TheCompDb.TableNamesL();
	for(TInt ii=0;ii<tables->Count();++ii)
		{
		CheckTableL(TheCompDb, TheCopyDb, (*tables)[ii]);
		}
	delete tables;
	TheCopyDb.Close();
	TheCompDb.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0593
@SYMTestCaseDesc        Database compression tests.
@SYMTestPriority        Medium
@SYMTestActions         Tests for RDbStoreDatabase::CompressL(),RDbStoreDatabase::DecompressL() functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void Test(const TDesC& aSource, const TDesC& aTarget, TBool aCompress)
	{
	TRAPD(err, CompressL(aSource, aTarget, aCompress));
	TEST2(err, KErrNone);
	TheTest.Printf(_L("Checking database"));
	TRAP(err, CheckL(aSource, aTarget));
	TEST2(err, KErrNone);
	}

void DoTests()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0593 Compressing..."));
	Test(TheBenchDbFileName, TheCompressedFileName, ETrue);
	
	TheTest.Next(_L("Decompressing..."));
	Test(TheCompressedFileName, TheDecompressedFileName, EFalse);
	}

//Usage: "t_comp [<drive letter>:]]"
TInt E32Main()
    {
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	TBuf<256> cmdline;
	User::CommandLine(cmdline);

	TParse parse;

	_LIT(KBenchDbFile, "C:\\DBMS-TST\\T_BENCH.DB");
	parse.Set(cmdline, &KBenchDbFile, 0);
	TheBenchDbFileName.Copy(parse.FullName());

	_LIT(KCompressedFile, "C:\\DBMS-TST\\T_COMP.DB1");
	parse.Set(cmdline, &KCompressedFile, 0);
	TheCompressedFileName.Copy(parse.FullName());

	_LIT(KDecompressedFile, "C:\\DBMS-TST\\T_COMP.DB2");
	parse.Set(cmdline, &KDecompressedFile, 0);
	TheDecompressedFileName.Copy(parse.FullName());
	
	__UHEAP_MARK;
	
	CreateTestEnv();
	DoTests();
	DestroyTestEnv();

	delete tc;

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	return 0;
    }
