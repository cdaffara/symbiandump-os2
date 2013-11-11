// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// test code for RTextBuf - DEF015702
// 
//

#include <bamatch.h>
#include <e32test.h>

LOCAL_D RTest test(_L("RTextBuf"));

const TInt KErrSimulated = -100001;
_LIT(KThreadName,"newThread");

void PanicFuncL()
  	{
  	RTextBuf buf;
  	CleanupClosePushL(buf);
  	buf.SetMaxLengthL(0);
  	CleanupStack::PopAndDestroy();
  	}

TInt ThreadFunc(TAny*)
  	{
  	CTrapCleanup* trapCleanup = CTrapCleanup::New();
  	TRAPD(err, PanicFuncL());
  	delete trapCleanup;
  	return err;
  	}

/**
@SYMTestCaseID          SYSLIB-BAFL-UT-1795
@SYMTestCaseDesc        Testing panic on RTextBuf class(JustInTimeDebug is disabled)
@SYMTestPriority        Low
@SYMTestActions         Test that panics, when the condition inside __ASSERT is made false, by passing 0 as parameter to SetMaxLengthL function
@SYMTestExpectedResults Tests must panic
@SYMREQ                 REQ0000
*/
void PanicTest()
  	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-UT-1795 "));
  	TRequestStatus threadStatus;
  	RThread thread;
  	TInt rc;
  	TBool jit;
  	jit = User::JustInTime();
  	User::SetJustInTime(EFalse);

  	rc = thread.Create(KThreadName, ThreadFunc,
  		KDefaultStackSize, KMinHeapSize, KMinHeapSize*4,NULL);
 	test(KErrNone == rc);

  	thread.Logon(threadStatus);
  	thread.Resume();

  	User::WaitForRequest(threadStatus);
  	User::SetJustInTime(jit);

  	test(thread.ExitType() == EExitPanic);
  	thread.Close();
  	}

/*

@SYMTestCaseID          SYSLIB-BAFL-CT-0466
@SYMTestCaseDesc        Tests for Defect number DEF015702
@SYMTestPriority        High
@SYMTestActions         Tests for clean completion even after Leave
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void DEF015702()
/**
 *  Test DEF015702.
 *  RTextBuf is not safe.
 *  RTextBuf has a destructor; but the destructor will not be called if a leave occurs
 *  (if the RTextBuf is on the stack).
 */
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0466 Testing fix for defect DEF015702 "));

	RTextBuf buf;

	CleanupClosePushL(buf);

	buf.SetMaxLengthL(20);
	//Test that raises a panic
	PanicTest();
	buf.SetText(_L("0123456789"));

	// Simulate a problem with calling of User::Leave(KErrSimulated).
	// There should be no "Panic ALLOC" in epocwind.out file.
	//And te test should be completed with OK.
	User::Leave(KErrSimulated);
	}


void DoE32MainL()
	{
	//
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	TRAPD(error, DEF015702());
	delete trapCleanup;
#ifdef _DEBUG
	TUint32 badCell = __UHEAP_MARKEND;
#endif
	if(error == KErrSimulated)
		{
#ifdef _DEBUG
		if(badCell == 0)
#endif
			{
			test.End();
			User::After(3000000);
			}
		}
	//
	}


TInt E32Main()
    {
	__UHEAP_MARK;

	test.Title();

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	TRAPD(error, DoE32MainL());

	delete trapCleanup;

	test.Close();

	__UHEAP_MARKEND;

	return error;
    }
