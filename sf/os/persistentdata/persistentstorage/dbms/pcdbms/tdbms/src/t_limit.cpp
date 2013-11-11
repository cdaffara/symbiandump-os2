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
#include <e32test.h>
#include <s32file.h>

#include "crccheck.h"

#undef __UHEAP_MARK
#define __UHEAP_MARK
#undef __UHEAP_MARKEND
#define __UHEAP_MARKEND

LOCAL_D TDBMS_CRCChecks TheCrcChecker;

#ifndef __linux__ //No CRC test on LINUX
#ifdef __TOOLS2__
const TPtrC	KCrcRecord=_L("\\epoc32\\winscw\\c\\dbms-tst\\T_LIMIT.CRC");
#else
const TPtrC	KCrcRecord=_L("C:\\dbms-tst\\T_LIMIT.CRC");
#endif
#endif


LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RFs TheFs;
LOCAL_D RDbs TheDbs;
LOCAL_D RDbNamedDatabase TheDatabase;
LOCAL_D RDbTable TheTable;
LOCAL_D RDbView TheView;

const TInt KTestCleanupStack=0x20;

#ifdef __TOOLS2__
const TPtrC KTestDatabase=_L(".\\dbms-tst\\T_LIMIT.DB");
#else
const TPtrC KTestDatabase=_L("C:\\dbms-tst\\T_LIMIT.DB");
#endif

const TPtrC KTableName(_S("TestTable"));

const TPtrC KColFormat=_L("c%d");

LOCAL_D RTest test(_L("T_LIMIT - testing table limits"));

const TInt KRecordLimit=8200;
const TInt KMinInlineLimit=16;
const TInt KMaxInlineLimit=255;

// expected maxima for record structure
const TInt KMaxColInt64NN=1025;
const TInt KMaxColText8=32;
const TInt KMaxColText16=16;
const TInt KMaxColLongText8=504;


LOCAL_C TBool FitBlob(TInt aCount)
//
// Matches heuristics in DBMS
//
	{
	TInt used=(aCount*(2+(KMinInlineLimit<<3))+7)>>3;
	return used<=KRecordLimit;
	}

LOCAL_C TInt InlineLimit(TInt aCount)
//
// Matches heuristics in DBMS
//
	{
	TInt used=(aCount*(2+(KMinInlineLimit<<3))+7)>>3;
	TInt space=(KRecordLimit-used);//>>1;
	TInt inl=space/aCount+KMinInlineLimit-1;
	return Min(inl,KMaxInlineLimit);
	}

LOCAL_C void OpenDatabase()
//
// Open the database
//
	{
	test (TheDatabase.Open(TheDbs,KTestDatabase)==KErrNone);
	}

LOCAL_C void CloseDatabase()
	{
	TheDatabase.Close();
	TheCrcChecker.GenerateCrcL(KTestDatabase);
	}

LOCAL_C void CreateDatabase()
//
// Create the database-in-a-store
//
	{
	test (TheDatabase.Replace(TheFs,KTestDatabase)==KErrNone);
	CloseDatabase();
	OpenDatabase();
	}

LOCAL_C void DestroyDatabase()
	{
	test (TheDatabase.Destroy()==KErrNone);
	}

LOCAL_C CDbColSet* SetLC(TDbCol& aCol,TInt aCount)
	{
	CDbColSet* set=CDbColSet::NewLC();
	TDbColName name;
	while (--aCount>=0)
		{
		name.Format(KColFormat,aCount);
		aCol.iName=name;
		set->AddL(aCol);
		}
	return set;
	}

LOCAL_C TBool TestL(TDbCol& aCol,TInt aCount)
	{
	test.Printf(_L("\rtesting %d    "),aCount);
	CDbColSet* set=SetLC(aCol,aCount);
	TInt r;
	r=TheDatabase.CreateTable(KTableName,*set);
	if (r==KErrNone)
		{
		CDbColSet* comp=TheDatabase.ColSetL(KTableName);
		test (comp->Count()==aCount);
		delete comp;
		}
	CleanupStack::PopAndDestroy();
	if (r==KErrTooBig)
		return EFalse;
	test (r==KErrNone);
	test (TheDatabase.DropTable(KTableName)==KErrNone);
	return ETrue;
	}

/**
See how many columns of this sort can be used

@SYMTestCaseID          SYSLIB-DBMS-CT-0631
@SYMTestCaseDesc        Tests for maximum limits on a Table 
@SYMTestPriority        Medium
@SYMTestActions         Tests for creating a table with maximum number of columns 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C TInt TestTypeL(TDbCol& aCol)
	{
	CreateDatabase();
	TInt ii=1;
	for (;;)
		{
		if (!TestL(aCol,ii))
			break;
		ii<<=1;
		}
	TInt lim=ii>>=1;
	test (lim>0);
	while ((ii>>=1)>0)
		{	// ok<=max<ok+ii*2
		if (TestL(aCol,lim+ii))
			lim+=ii;
		}
	DestroyDatabase();
	test.Printf(_L("\r   create %d     \n"),lim);
	return lim;
	}

LOCAL_C void StretchRecordL()
	{
	CreateDatabase();
	TDbCol col;
	col.iType=EDbColLongText8;
	col.iMaxLength=KDbUndefinedLength;
	col.iAttributes=0;
	for (TInt ii=4;FitBlob(ii);ii+=4)
		{
		test.Printf(_L("\rtesting %d    "),ii);
		CDbColSet* set=SetLC(col,ii);
		if (ii==4)
			test (TheDatabase.CreateTable(KTableName,*set)==KErrNone);
		else
			test (TheDatabase.AlterTable(KTableName,*set)==KErrNone);
		CleanupStack::PopAndDestroy();
		test (TheTable.Open(TheDatabase,KTableName)==KErrNone);
		if (ii==4)
			TheTable.InsertL();
		else
			{
			TheTable.FirstL();
			TheTable.UpdateL();
			}
		TBuf8<256> buf;
		buf.Fill('-',InlineLimit(ii)/*>>1*/);
		TPtrC8 ptr((const TUint8*)buf.Ptr(),buf.Size());
		TheTable.SetColL(ii-3,ptr);
		TheTable.SetColL(ii-2,ptr);
		TheTable.SetColL(ii-1,ptr);
		TheTable.SetColL(ii,ptr);
		TheTable.PutL();
		TheTable.Close();
//		if ((ii&0x1c)==0)
//			test (TheDatabase.Compact()==KErrNone);
		}
	test (TheDatabase.Compact()==KErrNone);
	test.Printf(_L("\n"));
	CloseDatabase();
	}

LOCAL_C void doMain()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0631 TInt64 NOT NULL "));
	TDbCol col;
	col.iType=EDbColInt64;
	col.iMaxLength=KDbUndefinedLength;
	col.iAttributes=TDbCol::ENotNull;
	test (TestTypeL(col)==KMaxColInt64NN);
	test.Next(_L("Text8"));
	col.iType=EDbColText8;
	col.iAttributes=0;
	test (TestTypeL(col)==KMaxColText8);
	test.Next(_L("Text16"));
	col.iType=EDbColText16;
	test (TestTypeL(col)==KMaxColText16);
	test.Next(_L("LongText8"));
	col.iType=EDbColLongText8;
	test (TestTypeL(col)==KMaxColLongText8);
	test.Next(_L("Stretching the record"));
	StretchRecordL();
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
#ifndef __TOOLS2__
	test (TheDbs.Connect()==KErrNone);
#endif
	__UHEAP_MARK;
//
	TRAPD(r,doMain();)
	test(r==KErrNone);

	test.Printf(_L("Waiting for server exit\n"));
	const TUint KExitDelay=6*0x100000;	// ~6 seconds
	User::After(KExitDelay);
	
	::DeleteDataFile(KTestDatabase);	//deletion of data files must be done before call to end - DEF047652
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

#ifndef __TOOLS2__
	TheDbs.Close();
#endif
	TheFs.Close();
	test.Close();
	return 0;
    }
