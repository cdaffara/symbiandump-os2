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
const TPtrC	KCrcRecord=_L("\\epoc32\\winscw\\c\\dbms-tst\\T_COMP.CRC");
#else
const TPtrC	KCrcRecord=_L("C:\\dbms-tst\\T_COMP.CRC");
#endif
#endif

LOCAL_D RTest test(_L("T_COMP"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RFs TheFs;

const TInt KTestCleanupStack=0x20;

#ifdef __TOOLS2__
const TPtrC KTestDir=_L(".\\dbms-tst\\");
#else
const TPtrC KTestDir=_L("C:\\dbms-tst\\");
#endif

//T_BENCH.DB file is created by T_BENCH test and is used by the current test (T_COMP).
//T_COMP test will delete T_BENCH.DB at the end as it is no more needed. 
//If you want to rerun T_COMP test again, you have to ensure that T_BENCH.DB file exists - 
//run T_BENCH test again.
_LIT(KTestFile,"T_BENCH.DB");

_LIT(KRomTestFile,"Z:\\TEST\\T_COMP.DAT");
_LIT(KCompressedFile,"T_COMP.DB1");
_LIT(KDecompressedFile,"T_COMP.DB2");

const TStreamId KHelpId=2;

//
// Compress the database
//
LOCAL_C void CompressL(const TDesC& aSource,const TDesC& aTarget,TBool aCompress)
	{
#ifndef __TOOLS2__
	CFileMan* man=CFileMan::NewL(TheFs);
	TInt r=man->Copy(aSource,aTarget);
	delete man;
	User::LeaveIfError(r);
#else
	TInt r;
    RFile f1;
    r = f1.Open(TheFs, aSource, EFileStreamText|EFileShareReadersOnly);
    test(r==KErrNone);
    RFile f2;
    r=f2.Replace(TheFs, aTarget, EFileWrite);
    test(r==KErrNone);

    TBuf8<512> copyBuf;
	TInt rem;
	r=f1.Size(rem);
	test(r==KErrNone);
	TInt pos=0;
	while (rem)
		{
		TInt s=Min(rem,copyBuf.MaxSize());
		r=f1.Read(pos,copyBuf,s);
		test(r==KErrNone);
		test(copyBuf.Length()==s);
		r=f2.Write(pos,copyBuf,s);
		test(r==KErrNone);
		pos+=s;
		rem-=s;
		}
	f1.Close();
	f2.Close();	
#endif
#ifndef __TOOLS2__
	User::LeaveIfError(TheFs.SetAtt(aTarget,0,KEntryAttReadOnly));
#endif
	CFileStore* store=CFileStore::OpenLC(TheFs,aTarget,EFileRead|EFileWrite);
	TUint t=User::TickCount();
	if (aCompress)
		RDbStoreDatabase::CompressL(*store,store->Root());
	else
		RDbStoreDatabase::DecompressL(*store,store->Root());
	t=User::TickCount()-t;
	test.Printf(_L("%d ticks\r\n"),t);
	store->CompactL();
	store->CommitL();
	CleanupStack::PopAndDestroy();	// store
	TInt err;
	TRAPD(lc, err = TheCrcChecker.GenerateCrcL(aTarget));
	test(err == KErrNone);
	test(lc == KErrNone);
	}

LOCAL_C void CheckTableL(RDbDatabase& aDatabase,RDbDatabase& aCopy,const TDesC& aTable)
	{
	test.Printf(_L("Processing table %S\n"),&aTable);
	RDbTable table;
	test (table.Open(aDatabase,aTable,table.EReadOnly)==KErrNone);
	RDbTable copy;
	test (copy.Open(aCopy,aTable,table.EReadOnly)==KErrNone);
	TInt columns=table.ColCount();
	while (table.NextL())
		{
		table.GetL();
		test (copy.NextL());
		copy.GetL();
		for (TInt ii=1;ii<=columns;++ii)
			{
			if (TDbCol::IsLong(table.ColType(ii)))
				{
				TInt len=table.ColSize(ii);
				test (len==copy.ColSize(ii));
				RDbColReadStream strm1;
				strm1.OpenLC(table,ii);
				RDbColReadStream strm2;
				strm2.OpenLC(copy,ii);
				TBuf8<512> buf1;
				TBuf8<512> buf2;
				while (len)
					{
					TInt block=Min(512,len);
					strm1.ReadL(buf1,block);
					strm2.ReadL(buf2,block);
					test (buf1==buf2);
					len-=block;
					}
				CleanupStack::PopAndDestroy(2);
				}
			else
				{
				test (table.ColDes8(ii)==copy.ColDes8(ii));
				}
			}
		}
	table.Close();
	copy.Close();
	}

LOCAL_C void CheckL(const TDesC& aSource,const TDesC& aTarget)
	{
	RDbNamedDatabase comp;
	RDbNamedDatabase copy;
	test (comp.Open(TheFs,aSource,TPtrC(),comp.EReadOnly)==KErrNone);
	test (copy.Open(TheFs,aTarget,TPtrC(),copy.EReadOnly)==KErrNone);
	CDbTableNames* tables=comp.TableNamesL();
	for (TInt ii=0;ii<tables->Count();++ii)
		CheckTableL(comp,copy,(*tables)[ii]);
	delete tables;
	copy.Close();
	comp.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0593
@SYMTestCaseDesc        Database compression tests.
@SYMTestPriority        Medium
@SYMTestActions         Tests for RDbStoreDatabase::CompressL(),RDbStoreDatabase::DecompressL() functions 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test(const TDesC& aSource,const TDesC& aTarget,TBool aCompress)
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0593 Converting database "));
	TRAPD(r,CompressL(aSource,aTarget,aCompress));
	test (r==KErrNone);
	test.Next(_L("Checking database"));
	TRAP(r,CheckL(aSource,aTarget));
	test (r==KErrNone);
	test.End();
	}

//
// Prepare the test directory.
//
LOCAL_C void setupTestDirectory()
    {
	TInt r=TheFs.Connect();
	test(r==KErrNone);
//
	r=TheFs.MkDir(KTestDir);
	test(r==KErrNone || r==KErrAlreadyExists);
	r=TheFs.SetSessionPath(KTestDir);
	test(r==KErrNone);
// On TOOLS2 - RFs::SetSessionPath() will affect all RFs Sessions, 
// the two RFs need same session path anyway
#ifdef __WINSCW__ 
	r=TheCrcChecker.SetSessionPath(KTestDir);
	test(r==KErrNone);
#endif
	}

//
// Initialise the cleanup stack.
//
LOCAL_C void setupCleanup()
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

//
// Test streaming conversions.
//
GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	test.Title();
	setupTestDirectory();
	setupCleanup();
//
	test.Start(_L("Compressing..."));
// Don't use the rom test file.
/*
	TUint dummy;
    TInt err = TheFs.Att(KRomTestFile,dummy);
	if (err==KErrNone)
		Test(KRomTestFile,KCompressedFile,ETrue);
	else
		Test(KTestFile,KCompressedFile,ETrue);
*/
	Test(KTestFile,KCompressedFile,ETrue);
	test.Next(_L("Decompressing..."));
	Test(KCompressedFile,KDecompressedFile,EFalse);
	
//

#ifndef __TOOLS2__
	_LIT(KTestDbName,"C:\\dbms-tst\\T_BENCH.DB");
	::DeleteDataFile(KTestDbName);
	_LIT(KTestDbName1,"C:\\dbms-tst\\T_COMP.DB1");
	::DeleteDataFile(KTestDbName1);
	_LIT(KTestDbName2,"C:\\dbms-tst\\T_COMP.DB2");
	::DeleteDataFile(KTestDbName2);
#else
	_LIT(KTestDbName,"T_BENCH.DB");
	::DeleteDataFile(KTestDbName);
	_LIT(KTestDbName1,"T_COMP.DB1");
	::DeleteDataFile(KTestDbName1);
	_LIT(KTestDbName2,"T_COMP.DB2");
	::DeleteDataFile(KTestDbName2);
#endif

#ifndef __linux__
#ifdef CRC_COMP  // Exclude Crc checking for this test. It's out of scope.
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
#endif

	delete TheTrapCleanup;
	test.End(); //call to end must be after deletion of files - DEF047652
	
	TheFs.Close();
	test.Close();
	__UHEAP_MARKEND;
	return 0;
    }
