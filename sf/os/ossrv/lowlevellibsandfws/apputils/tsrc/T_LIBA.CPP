// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Started by BLB, May 1996
// Test comms device
// 
//

#include <e32test.h>
#include <f32file.h>
#include "T_LIBT.H"
#include <baliba.h>

LOCAL_D RTest test(_L("Unloader"));
LOCAL_D RFs TheFs;
_LIT(KLibraryName, "libtst.tpr");
typedef CLibTest* (*NewTestL)();
typedef TLibAssoc<CLibTest> TTestAssoc;

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0415
@SYMTestCaseDesc        CLibTest class functionality test
@SYMTestPriority        Medium
@SYMTestActions         Library tester
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void testLibClass(CLibTest* aTester)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0415 "));
	test(aTester->Test1()==1);
	test(aTester->Test2()==2);
	test(aTester->Test3()==3);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0416
@SYMTestCaseDesc        Tests for constant CLibTest class
@SYMTestPriority        Medium
@SYMTestActions         Library tester
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void testLibClassC(const CLibTest* aTester)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0416 "));
	test(aTester->Test1()==1);
	test(aTester->Test2()==2);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-1283
@SYMTestCaseDesc        Tests for RLibrary::Load(),RLibrary::Lookup() function
@SYMTestPriority        Medium
@SYMTestActions         Attempt to load a named DLL.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void LoadCreate(CLibTest*& aTest,RLibrary& aLib)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1283 "));
	TInt error=aLib.Load(KLibraryName);
	test(error==KErrNone);
	NewTestL createClass=(NewTestL)aLib.Lookup(1);
	aTest=(*createClass)();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0417
@SYMTestCaseDesc        Manual load of library test
@SYMTestPriority        Medium
@SYMTestActions         Tests for library through manual load.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void testManualLoad()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0417 Manual "));
	RLibrary library;
	CLibTest* tester;
	LoadCreate(tester,library);
	testLibClass(tester);
	testLibClassC(tester);
	delete(tester);
	library.Close();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0418
@SYMTestCaseDesc        Smart load of library test
@SYMTestPriority        Medium
@SYMTestActions         Tests for library through smart load.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void testSmartLoad()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0418 Smart "));
	RLibrary library;
	CLibTest* tester;
	LoadCreate(tester,library);
	testLibClass(tester);
	testLibClassC(tester);
	TTestAssoc assoc(library,tester);
	testLibClass(assoc.Ptr());
	testLibClassC(assoc.Ptr());
	assoc.Unload();
	assoc.Unload();
	//
	LoadCreate(tester,library);
	TTestAssoc assoc2;
	assoc2.Set(library,tester);
	//
//	Following will panic since not cleared first
//	LoadCreate(tester,library);
//	assoc2.Set(library,tester);
	//
//	Following will not be possible
//	delete(assoc2);
	assoc2.Unload();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0419
@SYMTestCaseDesc        Smart load of library test
@SYMTestPriority        Medium
@SYMTestActions         Tests for Leave,cleanupstack on Leave
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void testSmartLeaveL(TTestAssoc& aAssoc,TBool aLeave)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0419 "));
	CleanupStack::PushL((TCleanupItem)aAssoc);
	if (aLeave)
		User::Leave(KErrNone);
	CleanupStack::PopAndDestroy();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0420
@SYMTestCaseDesc        Wrapper function calling library tester functions
@SYMTestPriority        Medium
@SYMTestActions         Call up library tester functions
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void testSmartClass()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0420 Pointer functions "));
	RLibrary library;
	CLibTest* tester;
	LoadCreate(tester,library);
	TTestAssoc assoc(library,tester);
	testLibClass(assoc);
	testLibClassC(assoc);
	assoc.Unload();
	//
	LoadCreate(tester,library);
	assoc.Set(library,tester);
	TRAPD(error,testSmartLeaveL(assoc,ETrue));
	test(error==KErrNone);
	//
	LoadCreate(tester,library);
	assoc.Set(library,tester);
	TRAP(error,testSmartLeaveL(assoc,EFalse));
	test(error==KErrNone);
	}

GLDEF_C TInt E32Main()
    {
	test.Title();
	test.Start(_L("Unloader "));
	TInt error=TheFs.Connect();
	test(error==KErrNone);
	__UHEAP_MARK;
	CTrapCleanup *trapCleanup=CTrapCleanup::New();
	//
	testManualLoad();
	testSmartLoad();
	testSmartClass();
	//
	delete(trapCleanup);
	__UHEAP_MARKEND;
    test.End();
	test.Close();
	return(0);
    }
