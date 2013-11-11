// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test code for stringpool defects that have been fixed, to help prevent regression
// 
//

/* this fixes a MSVC link warning */
#ifdef __VC32__
#pragma comment (linker, "/opt:noref") 
#endif

#include <stringpool.h>
#include <f32file.h>
#include <e32test.h>

#include <stringpoolerr.h>

#define UNUSED_VAR(a) a = a

LOCAL_D CTrapCleanup*		TheTrapCleanup=NULL;

LOCAL_D CActiveScheduler*	TheActiveScheduler=NULL;

LOCAL_D RTest				test(_L("T_StringpoolDefect"));

_LIT8(KDummyString,			"Dummy String For Test");

//

class RStringPoolPanicTest
	{
public:
	RStringPoolPanicTest();
	~RStringPoolPanicTest();
	
	void DEF043985L();
	
private:
	// Functions to pass Thread creation process that invokes the defect
	static TInt DEF043985_StringTest_ThreadL(TAny*); 
	static TInt DEF043985_StringFTest_ThreadL(TAny*);
	static TInt DEF043985_IndexTest_Thread(TAny*);
	
	static void DEF043985_IndexTest_SubFunctionL();
	};

RStringPoolPanicTest::RStringPoolPanicTest()
	{
	// do nothing
	}

RStringPoolPanicTest::~RStringPoolPanicTest()
	{
	// do nothing
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0488
@SYMTestCaseDesc        Tests for defect number DEF043985
@SYMTestPriority        High
@SYMTestActions         Tests that the table is invalid when creating an RString
                        Check for panic    
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
TInt RStringPoolPanicTest::DEF043985_StringTest_ThreadL(TAny*)
	{
	__UHEAP_MARK;

	RStringPool stringPool;
	stringPool.OpenL();

	// Create a table and make it so it passes some internal tests
	// for validity as we are testing a particular internal test
	TStringTable testTable;
	testTable.iCaseSensitive = ETrue;
	testTable.iCount = 1;

	// Test for Panic
	RString testString = stringPool.String(0, testTable);

	// Test should never get this far as it Panics

	testString.Close();
	stringPool.Close();

	__UHEAP_MARKEND;

	return KErrNone;		
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0489
@SYMTestCaseDesc        Tests for defect number DEF043985
@SYMTestPriority        High
@SYMTestActions         Check for Panic
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
TInt RStringPoolPanicTest::DEF043985_StringFTest_ThreadL(TAny*)
	{
	__UHEAP_MARK;

	RStringPool stringPool;
	stringPool.OpenL();

	// Create a table and make it so it passes some internal tests
	// for validity as we are testing a particular internal test
	TStringTable testTable;
	testTable.iCaseSensitive = EFalse;
	testTable.iCount = 1;

	// Test for Panic
	RStringF testString = stringPool.StringF(0, testTable);
	
	// Test should never get this far as it Panics

	testString.Close();
	stringPool.Close();

	__UHEAP_MARKEND;

	return KErrNone;		
	}

/**
Subtest function

@SYMTestCaseID          SYSLIB-BAFL-CT-0490
@SYMTestCaseDesc        Tests for defect number DEF043985
@SYMTestPriority        High
@SYMTestActions         Tests that the table is invalid when creating a folded RString
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void RStringPoolPanicTest::DEF043985_IndexTest_SubFunctionL()
	{
	__UHEAP_MARK;

	RStringPool stringPool;
	stringPool.OpenL();

	// Create a table and make it so it passes some internal tests
	// for validity as we are testing a particular internal test
	TStringTable testTable;
	testTable.iCaseSensitive = ETrue;
	testTable.iCount = 1;

	RString testString = stringPool.OpenStringL(KDummyString());
	CleanupClosePushL(testString);
	
	// Test for Panic
	testString.Index(testTable);
	
	// Test should never get this far as it Panics
	
	CleanupStack::PopAndDestroy(&testString);
	stringPool.Close();
	
	__UHEAP_MARKEND;
	}
/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0491
@SYMTestCaseDesc        Tests for the functionality of RStringPool
@SYMTestPriority        High
@SYMTestActions         Tests that the table is invalid when accessing an RStrings index
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
TInt RStringPoolPanicTest::DEF043985_IndexTest_Thread(TAny*)
	{
	__UHEAP_MARK;

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	__ASSERT_ALWAYS(trapCleanup!=NULL, User::Invariant());

	TRAPD(err, DEF043985_IndexTest_SubFunctionL());
    UNUSED_VAR(err);
	
	delete trapCleanup;
	
	__UHEAP_MARKEND;

	return KErrNone;		
	}

/**
Unfound TStringTable in Bafl StringPool not treated correctly.
Check that the function panics when the specified table is not present.

@SYMTestCaseID          SYSLIB-BAFL-CT-0492
@SYMTestCaseDesc        Tests for the functionality of RStringPool
@SYMTestPriority        High
@SYMTestActions         Tests for panic when the specified table is not present
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void RStringPoolPanicTest::DEF043985L()
	{
	test.Next(_L("DEF043985L"));
	
#ifdef _DEBUG
	__UHEAP_MARK;

	TRequestStatus threadStatus;
	RThread thread;
	TInt rc;
	TBool jit;
	jit = User::JustInTime();
	User::SetJustInTime(EFalse);
	
	// Test the Panics for this defect
	
	// RString Test
	rc = thread.Create(_L("DEF043985_StringTest_Thread Panic Test"), 
					   RStringPoolPanicTest::DEF043985_StringTest_ThreadL,
					   KDefaultStackSize, KMinHeapSize, KMinHeapSize<<2, this);
	test(KErrNone == rc);
	thread.Logon(threadStatus);
	thread.Resume();
	User::WaitForRequest(threadStatus);
	test (thread.ExitType() == EExitPanic);
	test (thread.ExitReason() == StringPoolPanic::EStringTableNotFound);
	thread.Close();

	// RStringF Test
	rc = thread.Create(_L("DEF043985_StringFTest_Thread Panic Test"), 
					   RStringPoolPanicTest::DEF043985_StringFTest_ThreadL,
					   KDefaultStackSize, KMinHeapSize, KMinHeapSize<<2, this);
	test(KErrNone == rc);
	thread.Logon(threadStatus);
	thread.Resume();
	User::WaitForRequest(threadStatus);
	test (thread.ExitType() == EExitPanic);
	test (thread.ExitReason() == StringPoolPanic::EStringTableNotFound);
	thread.Close();

	// Index Test
	rc = thread.Create(_L("DEF043985_IndexTest_Thread Panic Test"), 
					   RStringPoolPanicTest::DEF043985_IndexTest_Thread,
					   KDefaultStackSize, KMinHeapSize, KMinHeapSize<<2, this);
	test(KErrNone == rc);
	thread.Logon(threadStatus);
	thread.Resume();
	User::WaitForRequest(threadStatus);
	test (thread.ExitType() == EExitPanic);
	test (thread.ExitReason() == StringPoolPanic::EStringTableNotFound);
	thread.Close();


	User::SetJustInTime(jit);

	__UHEAP_MARKEND;
	
#else
	test.Printf(_L("This test is valid for debug builds only, behaviour for release builds is undefined (DEF050908)\n"));
#endif
	}

//===============================================================================

/**
Initialise the cleanup stack and active scheduler
*/
LOCAL_C void SetupL()
    {    
	TheTrapCleanup = CTrapCleanup::New();
	User::LeaveIfNull(TheTrapCleanup);

	// Construct and install the active scheduler
	TheActiveScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(TheActiveScheduler);
	}

/**
Cleanup
*/
LOCAL_C void CleanupL()
    {
	delete TheActiveScheduler;
	delete TheTrapCleanup;
	}

/**
Invoke the tests
*/
LOCAL_C void DoTestsL()
    {
	RStringPoolPanicTest mytest;	
	mytest.DEF043985L();
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start(_L("Defect Tests"));
	
	TRAPD(err, SetupL()); 
	test(err == KErrNone);
	
	TRAP(err, DoTestsL());
	test(err == KErrNone);

	CleanupL();
	
	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return(KErrNone);
	}
