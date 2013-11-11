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

#include "TestManager.h"


CTestManager::CTestManager(RPointerArray<CComponentInfo>* aTestList, 
						   CDataLogger& aDataLogger,
						   MManagerObserver& aObserver,
						   RTest* aRTest)
: CActive(CActive::EPriorityStandard),
iObserver(aObserver),
iTestList(aTestList),
iDataLogger(aDataLogger),
iRTest(aRTest)
	{
	}


CTestManager* CTestManager::NewL(RPointerArray<CComponentInfo>* aTestList, 
								 CDataLogger& aDataLogger,
								 MManagerObserver& aObserver,
								 RTest* aRTest)
	{
	CTestManager* self = new (ELeave) CTestManager(aTestList, aDataLogger, aObserver, aRTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


void CTestManager::ConstructL()
	{
	CActiveScheduler::Add(this);

	// Say we are stopping just in case RunTests never gets called
	// - if it does get called this will get unset
	iAmStopping = ETrue;
	}


CTestManager::~CTestManager()
	{
	Cancel();

	delete iCurrentTester;
	}


void CTestManager::RunL()
	{
	delete iCurrentTester;
	iCurrentTester = NULL;

	if((iCurrentTestLoad < iTestList->Count()) && !iAmStopping)
		{
		iStatus = KRequestPending;
		SetActive();

		TestComponentL(iCurrentTestLoad);
		
		// Next time run the next test
		++iCurrentTestLoad;
		// Set the flag for the next state.
		iAmStopping = iCurrentTestLoad == iTestList->Count();
		}
	else if(iAmStopping)
		iObserver.TestsComplete();
	}

TInt CTestManager::RunError(TInt /*aErrorCode*/)
	{
	// Do nothing because anything that needs to be cleaned up should be on the cleanup
	// stack. We want any remaining tests to carry on.
	return KErrNone;
	}


void CTestManager::DoCancel()
	{
	_LIT(KTestsCancelled,"TestBed cancelled at user request.");
	iDataLogger.LogInformation(KTestsCancelled());
	iDataLogger.ReportInformation(KTestsCancelled());

	delete iCurrentTester;
	iCurrentTester = NULL;

	iObserver.TestsComplete();
	}


void CTestManager::RunTests(RPointerArray<TTestInfo>* aTests)
	{
	iTestsToRun = aTests;

	if(iTestList->Count() >0)
		iAmStopping = EFalse;
	else
		{
		// If someone tried to call RunTests when there are no tests
		// complete immediately
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}

	if(!IsActive())
		{
		SetActive();
		if(!iAmStopping)
			{
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		else
			iStatus = KRequestPending;
		}
	}

/**
	@fn				CleanupTestArray(TAny* aArray)
	Intended Useage:The CleanupTestArray method is used for cleanup support 
					of locally declared arrays
	@internalComponent
	@since			7.0
	@param			aArray is the array whose contents should be destroyed
*/
static void CleanupTestArray(TAny* aArray)
	{
	// Whilst this array is an RPointerArray, it does not own the pointers
	// and therefor should not destroy them
	// This should be changed to an RArray
	RPointerArray<TTestInfo>* array = REINTERPRET_CAST(RPointerArray<TTestInfo>*, aArray);
	array->Reset();
	delete array;
	}


void CTestManager::Complete(CComponentTester* /*aTester*/, TInt /*aUnitTestId*/)
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

void CTestManager::TestComponentL(TInt aComponentIndex)
	{
	// This should be changed to an RArray<TTestInfo*> and be typedefd
	RPointerArray<TTestInfo>* tests = NULL;
	if(iTestsToRun != NULL)
		{
		tests = new(ELeave) RPointerArray<TTestInfo>;
		TCleanupItem cleanup(CleanupTestArray, tests);
		CleanupStack::PushL(cleanup);
		// Work out which tests to run
		for(TInt index = 0; index < iTestsToRun->Count(); ++index)
			{
			if((*iTestsToRun)[index]->iComponentId == aComponentIndex)
				User::LeaveIfError(tests->Append((*iTestsToRun)[index]));
			}
		if(tests->Count() == 0)
			{
			CleanupStack::PopAndDestroy();	// cleanup
			Complete(NULL, 0);
			return;
			}
		}
	// Create the EXEs derived CComponentTester for this test iteration.
	ComponentTesterInitialiserLC createLC = (*iTestList)[aComponentIndex]->GlobalEntryFunc();
	iCurrentTester= createLC(iDataLogger, *this);
	CleanupStack::Pop(iCurrentTester);
	iCurrentTester->SetRTest(iRTest);

	if(iTestsToRun != NULL)
		CleanupStack::Pop();				// cleanup
	// Execute unit tests for the current component
	iCurrentTester->TestComponent(tests);
	}

TBool CTestManager::StartedTests() const
	{
	return iCurrentTestLoad > 0;
	}

