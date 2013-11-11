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

#include "crccheck.h"

#undef __UHEAP_MARK
#define __UHEAP_MARK
#undef __UHEAP_MARKEND
#define __UHEAP_MARKEND

LOCAL_D TDBMS_CRCChecks TheCrcChecker;

#ifndef __linux__ //No CRC test on LINUX
#ifdef __TOOLS2__
const TPtrC	KCrcRecord=_L("\\epoc32\\winscw\\c\\dbms-tst\\T_TRANS.CRC");
#else
const TPtrC	KCrcRecord=_L("C:\\dbms-tst\\T_TRANS.CRC");
#endif
#endif

LOCAL_D RTest test(_L("T_TRANS - Test DBMS transactions"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RDbTable TheTable;
LOCAL_D RFs TheFs;
#ifndef __TOOLS2__
LOCAL_D RDbs TheDbs;
#endif
LOCAL_D RDbNamedDatabase TheDatabase;

const TInt KTestCleanupStack=0x20;

#ifdef __TOOLS2__
const TPtrC KTestDatabase=_L(".\\dbms-tst\\T_TRANS.DB");
#else
const TPtrC KTestDatabase=_L("C:\\dbms-tst\\T_TRANS.DB");
#endif

const TPtrC KTableName(_S("table"));
const TPtrC KIndexInt=_S("int");
const TPtrC KIndexText=_S("text");
const TPtrC KColumnInt=_S("int");
const TPtrC KColumnText=_S("text");
const TPtrC KColumnComment=_S("comment");
const TPtrC KCommentValue=_S("abcdefghijklmnopqrstuvwxyz");
const TInt KRecords=2000;

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
		test.Printf(_L("."));
		iPos=next;
		}
	if (aStep==0)
		test.Printf(_L("\n"));
	}

LOCAL_C void ProgressInc(RDbIncremental& inc,TInt aCount)
	{
	Progress progress(aCount);
	while (aCount)
		{
		inc.Next(aCount);
		progress.Next(aCount);
		}
	inc.Close();
	}

LOCAL_C void CreateDatabaseL()
//
// Create the database
//
	{
	TInt r=TheDatabase.Replace(TheFs,KTestDatabase);
	test (r==KErrNone);
	}

LOCAL_C void OpenDatabase()
//
// Create the database
//
	{
	TInt r=TheDatabase.Open(TheFs,KTestDatabase);
	test (r==KErrNone);
	}

LOCAL_C void CloseDatabaseL()
	{
	TheDatabase.Close();
	TheCrcChecker.GenerateCrcL(KTestDatabase);
	}

LOCAL_C void CreateTable()
	{
	TInt r=TheDatabase.Execute(_L("create table table (int integer,text varchar(8),comment varchar)"));
	test (r==KErrNone);
	}

LOCAL_C void WriteRecords(TInt aCount)
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

LOCAL_C TUint FileSize()
	{
#ifndef __TOOLS2__
	TEntry entry;
	test(TheFs.Entry(KTestDatabase,entry)==KErrNone);
	return entry.iSize;
#else
	RFile myfile;
	test(myfile.Open(TheFs, KTestDatabase, EFileRead) == KErrNone);
	TInt size;
	test(myfile.Size(size) == KErrNone);
	myfile.Close();
	return size;
#endif
	}

LOCAL_C void BuildTable(TInt aCount,TBool aTransactions,TUint& aTime,TUint& aSize)
	{
	TUint size=FileSize();
	TUint time=User::TickCount();
	CreateTable();
	if (aTransactions)
		TheDatabase.Begin();
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	WriteRecords(aCount);
	if (aTransactions)
		test(TheDatabase.Commit()==KErrNone);
	TheTable.Close();
	aTime=User::TickCount()-time;
	aSize=FileSize()-size;
	}

LOCAL_C void Execute(const TDesC& aSql)
	{
	RDbIncremental inc;
	TInt step;
	test(inc.Execute(TheDatabase,aSql,step)==KErrNone);
	ProgressInc(inc,step);
	}

LOCAL_C void BreakIndex()
	{
	TheDatabase.Begin();
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	TheTable.InsertL();
	TheTable.SetColL(1,-1);
	TheTable.PutL();
	TheTable.Close();
	TheDatabase.Rollback();
	test(TheDatabase.IsDamaged());
	}

LOCAL_C void Recover()
	{
	RDbIncremental rec;
	TInt step;
	test(rec.Recover(TheDatabase,step)==KErrNone);
	ProgressInc(rec,step);
	test(!TheDatabase.IsDamaged());
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0637
@SYMTestCaseDesc        Streaming conversions test
@SYMTestPriority        Medium
@SYMTestActions         Test the database definition and enquiry functions 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0637 Build without transactions "));
	CreateDatabaseL();
	TUint time1,size1;
	BuildTable(KRecords,EFalse,time1,size1);
	CloseDatabaseL();
	test.Next(_L("Build with transactions"));
	CreateDatabaseL();
	TUint time2,size2;
	BuildTable(KRecords,ETrue,time2,size2);
	test.Printf(_L("Transaction performance: time %4.2f, size %4.2f\n"),TReal(time1)/TReal(time2),TReal(size1)/TReal(size2));
	test.Next(_L("Build Int index"));
	Execute(_L("create unique index int on table (int)"));
	test.Next(_L("Break index"));
	BreakIndex();
	test.Next(_L("Build Text index"));
	Execute(_L("create unique index text on table (text)"));
	test.Next(_L("Recover"));
	test (TheDatabase.IsDamaged());
	CloseDatabaseL();
	OpenDatabase();
	test (TheDatabase.IsDamaged());
	Recover();
	test.Next(_L("Drop table"));
	Execute(_L("drop table table"));
	CloseDatabaseL();
	}

LOCAL_C void setupTestDirectory()
//
// Prepare the test directory.
//
    {
	TInt r=TheFs.Connect();
	test(r==KErrNone);
//
	r=TheFs.MkDir(KTestDatabase);
	test(r==KErrNone || r==KErrAlreadyExists);
	}

LOCAL_C void setupCleanup()
//
// Initialise the cleanup stack.
//
    {
	TheTrapCleanup=CTrapCleanup::New();
	test(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)0);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	test(r==KErrNone);
	}

LOCAL_C void DeleteDataFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
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
	}


GLDEF_C TInt E32Main()
    {
	test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;

	TRAPD(r,Test();)
	test(r==KErrNone);

	test.Printf(_L("Waiting for server exit\n"));
	const TUint KExitDelay=6*0x100000;	// ~6 seconds
	User::After(KExitDelay);
	
	//deletion of data files must be done before call to End() - DEF047652
	::DeleteDataFile(KTestDatabase);

#ifndef __linux__
	TInt err;
#ifndef __TOOLS2__
	TRAPD(lc, err = TheCrcChecker.DumpCrcRecordsL(KCrcRecord));
	test(err==KErrNone);
	test(lc==KErrNone);
#else
	TRAPD(lc, err = TheCrcChecker.ValidateCrcRecordsL(KCrcRecord));
	TPtrC errmsg;
	TheCrcChecker.ErrorReportL(err, errmsg);
	RDebug::Print(errmsg);
	test(err==KErrNone || err==TDBMS_CRCChecks::ECrcCheckOk);
#endif
#endif

	test.End();
//
	__UHEAP_MARKEND;

	delete TheTrapCleanup;
	TheFs.Close();
	test.Close();
	return 0;
    }
