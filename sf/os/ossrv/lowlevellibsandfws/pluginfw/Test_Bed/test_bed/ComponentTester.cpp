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
//

#include "ComponentTester.h"
#include <ecom/test_bed/datalogger.h>
#include <ecom/test_bed/testbeddefinitions.h>

EXPORT_C CComponentTester::CComponentTester(CDataLogger&			aDataLogger, 
											MComponentTestObserver& aObserver)
: CActive(CActive::EPriorityStandard),
iDataLogger(aDataLogger),
iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}


EXPORT_C CComponentTester::~CComponentTester()
	{
	Cancel();

	// This should only be true during if TransitionSetsL() left and we are being
	// destroyed as part of cleanup
	if(iTransitionSets)
		{
		iTransitionSets->ResetAndDestroy();
		delete iTransitionSets;
		}
	iParameterizedTests.Reset();
	if(iUnitTests)
		{
		iUnitTests->ResetAndDestroy();
		delete iUnitTests;
		}
	if(iUnitTestsToRun)
		{
		// We own the list but not the things on it.  So reset the list and delete it
		iUnitTestsToRun->Reset();
		delete iUnitTestsToRun;
		}
	}

EXPORT_C void CComponentTester::ComponentTesterConstructL()
	{
	iUnitTests = new(ELeave) RPointerArray<CUnitTest>;
	}


EXPORT_C RPointerArray<CUnitTestInfo>* CComponentTester::TransitionSetsL() const
	{
	// Create the array in a member variable to ensure correct cleanup but we do not
	// own this object.  Ownership is passed at the return
	iTransitionSets = new(ELeave) RPointerArray<CUnitTestInfo>;

	if(iUnitTests)
		{
		TInt numTests = iUnitTests->Count();
		for(TInt index = 0; index < numTests; index++)
			{
			CUnitTestInfo* newSet = (*iUnitTests)[index]->TransitionSetL();
			CleanupStack::PushL(newSet);
			User::LeaveIfError(iTransitionSets->Append(newSet));
			CleanupStack::Pop(newSet); // now owned by iTransitionSets
			}
		}

	// Return the pointer and null our member variable because we don't own it
	RPointerArray<CUnitTestInfo>* transitionSets = iTransitionSets;
	iTransitionSets = 0;
	return transitionSets;
	}

EXPORT_C void CComponentTester::Complete(CUnitTest* aUnitTest)
	{
	if(iUnitTestsToRun != 0)
		{
		if(iCurrentUnitTest == iUnitTestsToRun->Count())
			{
			iObserver.Complete(this, iUnitTests->Find(aUnitTest));
			}
		else
			{
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		}
	else
		{
		if(iCurrentUnitTest == iUnitTests->Count())
			{
			iObserver.Complete(this, iUnitTests->Find(aUnitTest));
			}
		else
			{
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		}
	}

EXPORT_C void CComponentTester::AddUnitTestL(const CUnitTest* aUnitTest)
	{
	CUnitTest* thisTest = CONST_CAST(CUnitTest*, aUnitTest);
	CleanupStack::PushL(thisTest);
	User::LeaveIfError(iUnitTests->Append(thisTest));
	CleanupStack::Pop(thisTest);
	}

EXPORT_C void CComponentTester::AddParamUnitTestL(const CUnitTest* aUnitTest)
	{
	CUnitTest* thisTest = CONST_CAST(CUnitTest*, aUnitTest);
	CleanupStack::PushL(thisTest);
	User::LeaveIfError(iUnitTests->Append(thisTest));
	CleanupStack::Pop(thisTest);
	TInt testId = iUnitTests->Find(thisTest);
	User::LeaveIfError(iParameterizedTests.Append(testId));
	}

EXPORT_C void CComponentTester::TestComponent(RPointerArray<TTestInfo>* aTests)
	{
	_LIT(KStartingTestMessage, "Starting CComponentTester::TestComponent()...");
	iDataLogger.LogInformation(KStartingTestMessage);

	iUnitTestsToRun = aTests;

	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

EXPORT_C void CComponentTester::RunL()
	{
	TBool lastTestRun = EFalse;
	if(iUnitTestsToRun == 0)
		{
		TBool startFromFirst = iCurrentUnitTest == 0;
		TBool haveRunTest = EFalse;
		while((iCurrentUnitTest < iUnitTests->Count()) && !haveRunTest)
			{
			if(iParameterizedTests.Find(iCurrentUnitTest) == -1)
				{
				(*iUnitTests)[iCurrentUnitTest]->PrepareUnitTestL();
				(*iUnitTests)[iCurrentUnitTest]->RunTest(0);
				haveRunTest = ETrue;
				}
			++iCurrentUnitTest;
			lastTestRun = iCurrentUnitTest == iUnitTests->Count();
			}
		if(startFromFirst && !haveRunTest)
			iObserver.Complete(this, KErrNotFound);
		}
	else
		{
		if(iCurrentUnitTest < iUnitTestsToRun->Count())
			{
			TInt testToRun = (*iUnitTestsToRun)[iCurrentUnitTest]->iUnitTestId;
			TTimeIntervalMicroSeconds32 time = (*iUnitTestsToRun)[iCurrentUnitTest]->iRunTime;
			(*iUnitTests)[testToRun]->SetParametersL((*iUnitTestsToRun)[iCurrentUnitTest]->iParameters);
			(*iUnitTests)[testToRun]->PrepareUnitTestL();
			(*iUnitTests)[testToRun]->RunTest(time);
			++iCurrentUnitTest;
			lastTestRun = iCurrentUnitTest == iUnitTestsToRun->Count();
			}
		}

	// We don't need to be active for the last test because we don't RunL again
	if(!lastTestRun)
		{
		iStatus = KRequestPending;
		SetActive();
		}
	}

EXPORT_C void CComponentTester::DoCancel()
	{
	// If we have started a test then we have already advanced iCurrentUnitTest so cancel 
	// the previous test
	if(iCurrentUnitTest > 0)
		(*iUnitTests)[iCurrentUnitTest - 1]->Cancel();

	iObserver.Complete(this, KTestBedTestCancel);
	}

EXPORT_C void CComponentTester::SetRTest(RTest* aRTest)
	{
	// Record a handle on the RTest object to use in component testing.
	iRTest = aRTest;

	// We have a new RTest, best tell the unit tests we know about
	for (int ut=0; ut < iUnitTests->Count(); ut++)
		(*iUnitTests)[ut]->SetRTest(aRTest);
	}

