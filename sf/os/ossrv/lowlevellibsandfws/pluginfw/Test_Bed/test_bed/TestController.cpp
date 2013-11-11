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

#include <e32std.h>
#include <e32uid.h>

#include "TestController.h"
#include "ComponentTester.h"
#include <ecom/test_bed/testbeddefinitions.h>
#include <ecom/test_bed/datalogger.h>


CTestController::CTestController(CActiveScheduler* aScheduler, RTest* aRTest)
: CBase(),
iScheduler(aScheduler),
iRTest(aRTest)
	{
	}


EXPORT_C CTestController::~CTestController()
	{
	// Cancel any outstanding tests
	Cancel();

	iTestList.ResetAndDestroy();
	delete iTestManager;
	if(iOwnScheduler)
		delete iScheduler;
	Dll::SetTls(NULL);
	delete iDataLogger;
	}


EXPORT_C CTestController* CTestController::NewLC(CActiveScheduler* aScheduler,
												 ComponentTesterInitialiserLC aEntryPoint,
												 RTest* aRTest,
												 TLoggingInfo* aLogInfo)
	{
	CTestController* self = new (ELeave) CTestController(aScheduler, aRTest);
	CleanupStack::PushL(self);
	self->ConstructL(aLogInfo, aEntryPoint);
	return self;
	}


EXPORT_C CTestController* CTestController::NewL(CActiveScheduler* aScheduler,
												ComponentTesterInitialiserLC aEntryPoint,
												RTest* aRTest,
												TLoggingInfo* aLogInfo)
	{
	CTestController* self = NewLC(aScheduler, aEntryPoint, aRTest, aLogInfo);
	CleanupStack::Pop();
	return self;
	}


void CTestController::ConstructL(TLoggingInfo* aLogInfo, ComponentTesterInitialiserLC aEntryPoint)
	{
	if(iScheduler == NULL)
		{
		  // Construct and install the active scheduler
		iScheduler = new (ELeave) CActiveScheduler;
		iOwnScheduler = ETrue;
		CActiveScheduler::Install(iScheduler); 
		}

	// Create a logging channel
	iDataLogger = CDataLogger::NewL(aLogInfo);
	Dll::SetTls(iDataLogger);

	// Create the component tester object required for testing
	InitialiseComponentTesterL(aEntryPoint);

	_LIT(KCreatingTestManager,"Creating a test manager");
	iDataLogger->LogInformation(KCreatingTestManager());

	iTestManager = CTestManager::NewL(&iTestList, *iDataLogger, *this, iRTest);
	}


EXPORT_C void CTestController::Start()
	{
	iTestManager->RunTests(NULL);
	iScheduler->Start();
	}

EXPORT_C void CTestController::Start(RPointerArray<TTestInfo>* aTests)
	{
	iTestManager->RunTests(aTests);
	iScheduler->Start();
	}

EXPORT_C void CTestController::Start(TRequestStatus* aStatus)
	{
	Start(aStatus, NULL);
	}

EXPORT_C void CTestController::Start(TRequestStatus* aStatus, RPointerArray<TTestInfo>* aTests)
	{
	__ASSERT_DEBUG(CActiveScheduler::Current(), User::Invariant());
	iClientStatus = aStatus;
	iTestManager->RunTests(aTests);
	}


EXPORT_C const RPointerArray<CComponentInfo>& CTestController::FindComponents() const
	{
	// Return the list of classes that can be tested
	return iTestList;
	}

/**
	@fn				CleanupArray(TAny* aArray)
	Intended Useage:The CleanupArray method is used for cleanup support 
					of locally declared arrays
	@internalComponent
	@since			7.0
	@param			aArray is the array whose contents should be destroyed
*/
static void CleanupArray(TAny* aArray)
	{
	RPointerArray<CUnitTestInfo>* array = 
		REINTERPRET_CAST(RPointerArray<CUnitTestInfo>*, aArray);
	array->ResetAndDestroy();
	delete array;
	}

void CTestController::InitialiseComponentTesterL(ComponentTesterInitialiserLC aEntryPointLC)
	{
	_LIT(KInitCompTester, "Initialising derived component tester object");
	iDataLogger->LogInformation(KInitCompTester());
	// Invoking the function passed in will result in a derived 
	// CComponentTester object being created and pushed on the clean up 
	// stack. Therefore we need to do a pop and destroy later.
	CComponentTester* componentTesterFromEXE =  aEntryPointLC(*iDataLogger,*iTestManager);
   
	_LIT(KCreateTranSets,"Creating component tester & Building Unit Test information.");
	iDataLogger->LogInformation(KCreateTranSets());
	RPointerArray<CUnitTestInfo>* unitTests = componentTesterFromEXE->TransitionSetsL();

	CleanupStack::PopAndDestroy(componentTesterFromEXE);	//componentTester as pushed by aEntryPoint

	TCleanupItem cleanup(CleanupArray, unitTests);
	CleanupStack::PushL(cleanup);
	
	CComponentInfo* info = CComponentInfo::NewL(aEntryPointLC, unitTests);
	CleanupStack::Pop(unitTests);	// unitTests, now owned by info
	CleanupStack::PushL(info);
	User::LeaveIfError(iTestList.Append(info));		// pass ownership onto the list.
	CleanupStack::Pop(info); // now owned by iTestList
	}

EXPORT_C CDataLogger& CTestController::DataLogger()
	{
	return *(REINTERPRET_CAST(CDataLogger*,Dll::Tls()));
	}

EXPORT_C void CTestController::Cancel()
	{
	if(iTestManager)
		{
		iTestManager->Cancel();

		if(!iTestManager->StartedTests() && iClientStatus)
			{
			User::RequestComplete(iClientStatus, KTestBedTestCancel);
			iClientStatus = NULL;
			}
		}
	}

void CTestController::TestsComplete()
	{
	if(iClientStatus)
		{
		User::RequestComplete(iClientStatus, KErrNone);
		iClientStatus = NULL;
		}
	else
		CActiveScheduler::Stop();
	}

