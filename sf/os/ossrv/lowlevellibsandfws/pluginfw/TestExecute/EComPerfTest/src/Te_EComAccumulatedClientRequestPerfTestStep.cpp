// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of ECom Test to determine performance of ECom client calls during different stages of start-up.
// 
//

/**
 @file
 @internalComponent
*/

#include "Te_EComAccumulatedClientRequestPerfTestStep.h"
#include <startup.hrh>
#include <ecom/ecom.h>
#include "Interface.h"
#include "EComSessionAux.h"
#include "EComPerformance.h"

CEComAccumulatedClientRequestsPerfTest::CEComAccumulatedClientRequestsPerfTest() : CEComPerfTestBase(KEComAccumulatedClientRequestsPerfTest)
	{
	}

CEComAccumulatedClientRequestsPerfTest::~CEComAccumulatedClientRequestsPerfTest()
	{
	}

/**
 This test will retrieve the timing servicing client requests for the start-up states in this list
 */
const TStartupStateIdentifier CEComAccumulatedClientRequestsPerfTest::iMeasuredStartupStates[] = 
	{
	EStartupStateCriticalStatic,
	EStartupStateCriticalDynamic,
	EStartupStateNonCritical
	};
const TInt CEComAccumulatedClientRequestsPerfTest::iNumMeasuredStartupStates = sizeof(iMeasuredStartupStates)/sizeof(iMeasuredStartupStates[0]);

/**
 Retrieves client request timer results from the ECom server
 then prints out timer results organized by startup state, client request type, and then all client requests
 */
TVerdict CEComAccumulatedClientRequestsPerfTest::doTestStepL()
	{
#ifdef __ECOM_SERVER_PERFORMANCE__
	// get all the timer results from the server
	RClientRequestTimerResults clientTimerResults;
	CleanupClosePushL(clientTimerResults);
	clientTimerResults.RetrieveResultsL();
	
	
	INFO_PRINTF1(_L("ECom client request start-up timing broken down by request type and startup state:\n"));
	//
	// ECom start-up timing broken down by request type and startup state
	// This loop goes through each start-up state in iMeasuredStartupStates, and for each client request type, retrieves the timing results from the clientTimerResults array
	//
	for (TInt startupStateIndex = 0; startupStateIndex < iNumMeasuredStartupStates; startupStateIndex++)
		{
		TPtrC startupStateName = StartupStateName(iMeasuredStartupStates[startupStateIndex]);
		for (TInt clientRequestTypeInt = 0; clientRequestTypeInt < EEComNumClientRequestTypes; clientRequestTypeInt++)
			{
			TEComClientRequestType clientRequestType = (TEComClientRequestType)clientRequestTypeInt;
			TPtrC clientRequestTypeName = ClientRequestTypeName(clientRequestType);
			
			TUint numRequestsByTypeAndStartupState = 0;
			TReal timeForRequestsByTypeAndStartupState = clientTimerResults.GetAccumulatedClientRequestTime(iMeasuredStartupStates[startupStateIndex], clientRequestType, numRequestsByTypeAndStartupState);
			INFO_PRINTF5(_L("Time servicing %S requests, %S State: [%f] msec, number of requests: [%d]"), 
				&clientRequestTypeName, &startupStateName,
				timeForRequestsByTypeAndStartupState, numRequestsByTypeAndStartupState);
			}
		}
	

	INFO_PRINTF1(_L("\nECom client request start-up timing broken down by request type:\n"));
	//
	// ECom start-up timing broken down by request type
	// This loop goes through each client request type, retrieves the timing results from the clientTimerResults array
	//
	for (TInt clientRequestTypeInt = 0; clientRequestTypeInt < EEComNumClientRequestTypes; clientRequestTypeInt++)
		{
		TEComClientRequestType clientRequestType = (TEComClientRequestType)clientRequestTypeInt;
		TPtrC clientRequestTypeName = ClientRequestTypeName(clientRequestType);

		TUint numRequestsByType = 0;
		TReal timeForRequestsByType = clientTimerResults.GetAccumulatedClientRequestTime(clientRequestType, numRequestsByType);
		INFO_PRINTF4(_L("Time servicing %S requests: [%f] msec, number of requests: [%d]"), 
			&clientRequestTypeName, timeForRequestsByType, numRequestsByType);
		}


	INFO_PRINTF1(_L("\nECom client request start-up timing broken down by start-up state:\n"));
	//
	// ECom start-up timing broken down by start-up state
	// This loop goes through start-up state in iMeasuredStartupStates, and retrieves the timing results from the clientTimerResults array
	//
	for (TInt startupStateIndex = 0; startupStateIndex < iNumMeasuredStartupStates; startupStateIndex++)
		{
		TPtrC startupStateName = StartupStateName(iMeasuredStartupStates[startupStateIndex]);
		TUint numRequestsByStartupState = 0;
		TReal timeForRequestsByStartupState = clientTimerResults.GetAccumulatedClientRequestTime(iMeasuredStartupStates[startupStateIndex], numRequestsByStartupState);
		INFO_PRINTF4(_L("Time servicing requests in %S State: [%f] msec, number of requests: [%d]"), 
			&startupStateName, timeForRequestsByStartupState, numRequestsByStartupState);
		}
	

	//
	// Total time servicing client requests
	// This loop goes retrieves the total time spent servicing client requests
	//
	TUint totalNumRequests = 0;
	TReal totalTimeForRequests = clientTimerResults.GetAccumulatedClientRequestTime(totalNumRequests);
	INFO_PRINTF3(_L("Total time servicing all requests: [%f] msec, number of requests: [%d]"), 
		totalTimeForRequests, totalNumRequests);
		
	if (totalNumRequests >= KAccumulatedClientRequestTimerResultMaxCount)
		{
		INFO_PRINTF2(_L("Number of client requests exceeded %d. Not all client requests may have been recorded\n"), KAccumulatedClientRequestTimerResultMaxCount);
		return EFail;
		}

	CleanupStack::PopAndDestroy(&clientTimerResults);

#else
	MacroNotDefinedError();
#endif // __ECOM_SERVER_PERFORMANCE__
	return TestStepResult();
	}
