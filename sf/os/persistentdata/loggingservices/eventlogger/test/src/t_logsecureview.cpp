// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_logutil2.h"
#include <logview.h>

#define PRECONDITION_TRUE(x) TEST((x))
#define PRECONDITION_EQ(x,y) TEST2((x),(y))

// If LOWCAP is defined in the .mmp file 'hiCapabilityTest' will be set to FALSE.
#ifdef LOWCAP
	TBool hiCapabilityTest = EFalse;
	RTest TheTest(_L("t_logsecureview_lowcap"));
	_LIT(KTestTitle, "t_logsecureview (low capability)");
#else
	TBool hiCapabilityTest = ETrue;
	RTest TheTest(_L("t_logsecureview_hicap"));
	_LIT(KTestTitle, "t_logsecureview (high capability)");
#endif


/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0135
@SYMTestCaseDesc Ensures only a capable client can remove an event from a recent list
@SYMTestActions See the description and expected results.
@SYMTestPriority High
@SYMTestExpectedResults Should always succeed
@SYMREQ REQ3431
*/
LOCAL_C void TestDuplicateRemoveIdL(CLogViewDuplicate* aView)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0135 "));
	TLogId id = aView->Event().Id();
	TRAPD(error, aView->RemoveL(id));
	
	if(hiCapabilityTest)
		{
		TEST2(error, KErrNone);
		TEST2(aView->CountL(), 3);
		}
	else
		{
		TEST2(error, KErrPermissionDenied);
		TEST2(aView->CountL(), 4);
		}
	}
	
/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0136
@SYMTestCaseDesc Ensures only a capable client can remove an event from a recent list
@SYMTestActions See the description and expected results.
@SYMTestPriority High
@SYMTestExpectedResults Should always succeed
@SYMREQ REQ3431
*/
LOCAL_C void TestDuplicateRemoveCurrentL(CLogViewDuplicate* aView, CTestActive* aTestActive)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0136 "));
	aTestActive->StartL();
	PRECONDITION_TRUE(aView->FirstL(aTestActive->iStatus));
	CActiveScheduler::Start();
	PRECONDITION_EQ(aTestActive->iStatus.Int(), KErrNone);

	if(hiCapabilityTest)
		{
		aTestActive->StartL();
		PRECONDITION_TRUE(aView->RemoveL(aTestActive->iStatus));
		CActiveScheduler::Start();
		PRECONDITION_EQ(aTestActive->iStatus.Int(), KErrNone);
		
		TEST2(aView->CountL(), 2);
		}
	else
		{
		TRAPD(error, aView->RemoveL(aTestActive->iStatus));
	
		TEST2(error, KErrPermissionDenied);
		TEST2(aView->CountL(), 4);
		}
	}

/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0137
@SYMTestCaseDesc Ensures only a capable client can clear duplicate events from a recent list
@SYMTestActions See the description and expected results.
@SYMTestPriority High
@SYMTestExpectedResults Should always succeed
@SYMREQ REQ3431
*/
LOCAL_C void TestRecentClearDuplicatesL(CLogViewRecent* aRecentView, CLogViewDuplicate* aDuplicateView)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0137 "));
	TRAPD(error, aRecentView->ClearDuplicatesL());

	if(hiCapabilityTest)
		{
		TEST2(error, KErrNone);
		TEST2(aDuplicateView->CountL(), 0);
		}
	else
		{
		TEST2(error, KErrPermissionDenied);
		TEST2(aDuplicateView->CountL(), 4);
		}
	}
	
/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0138
@SYMTestCaseDesc Ensures only a capable client can remove an event from a duplicate list
@SYMTestActions See the description and expected results.
@SYMTestPriority High
@SYMTestExpectedResults Should always succeed
@SYMREQ REQ3431
*/
LOCAL_C void TestRecentRemoveIdL(CLogViewRecent* aView)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0138 "));
	TLogId id = aView->Event().Id();
	TRAPD(error, aView->RemoveL(id));
	
	if(hiCapabilityTest)
		{
		TEST2(error, KErrNone);
		TEST2(aView->CountL(), 1);
		}
	else
		{
		TEST2(error, KErrPermissionDenied);
		TEST2(aView->CountL(), 2);
		}
	}

/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0139
@SYMTestCaseDesc Ensures only a capable client can remove an event from a duplicate list
@SYMTestActions See the description and expected results.
@SYMTestPriority High
@SYMTestExpectedResults Should always succeed
@SYMREQ REQ3431
*/
LOCAL_C void TestRecentRemoveCurrentL(CLogViewRecent* aView, CTestActive* aTestActive)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0139 "));
	aTestActive->StartL();
	PRECONDITION_TRUE(aView->FirstL(aTestActive->iStatus));
	CActiveScheduler::Start();
	PRECONDITION_EQ(aTestActive->iStatus.Int(), KErrNone);

	TRAPD(error, aView->RemoveL(aTestActive->iStatus));

	if(hiCapabilityTest)
		{
		TEST2(error, KErrNone);
		TEST2(aView->CountL(), 0);
		}
	else
		{
		TEST2(error, KErrPermissionDenied);
		TEST2(aView->CountL(), 2);
		}
	}
	
LOCAL_C void TestViewsL()
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CTestActive* testActive = new(ELeave)CTestActive();
	CleanupStack::PushL(testActive);

	CLogViewRecent* recentView = CLogViewRecent::NewL(*client);
	CleanupStack::PushL(recentView);

	testActive->StartL();
	PRECONDITION_TRUE(recentView->SetRecentListL(KLogRecentIncomingCalls, testActive->iStatus));
	CActiveScheduler::Start();
	PRECONDITION_EQ(testActive->iStatus.Int(), KErrNone);
	PRECONDITION_EQ(recentView->CountL(), 2);

	CLogViewDuplicate* duplicateView = CLogViewDuplicate::NewL(*client);
	CleanupStack::PushL(duplicateView);

	testActive->StartL();
	PRECONDITION_TRUE(recentView->DuplicatesL(*duplicateView, testActive->iStatus));
	CActiveScheduler::Start();
	PRECONDITION_EQ(testActive->iStatus.Int(), KErrNone);
	PRECONDITION_EQ(duplicateView->CountL(), 4);
	
	TheTest.Start(_L("RemoveL on duplicate view with id as argument"));
	TestDuplicateRemoveIdL(duplicateView);

	TheTest.Next(_L("RemoveL on duplicate view at current cursor position"));
	TestDuplicateRemoveCurrentL(duplicateView, testActive);

	TheTest.Next(_L("ClearDuplicatesL on recent view"));
	TestRecentClearDuplicatesL(recentView, duplicateView);

	TheTest.Next(_L("RemoveL on recent view with id as argument"));
	TestRecentRemoveIdL(recentView);
	
	TheTest.Next(_L("RemoveL on recent view at current cursor position"));
	TestRecentRemoveCurrentL(recentView, testActive);

	CleanupStack::PopAndDestroy(4);
	}

//.............................................................................

void doTestsL()
	{
	TestUtils::Initialize(KTestTitle);
	TestUtils::DeleteDatabaseL();
	TestUtils::AddViewTestEventsL();

	TestViewsL();

	TestUtils::DeleteDatabaseL();
	}
