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
// Overload of the CTransition test that provides
// Heap and Handle leak testing upon a test method.
// 
//

#include <ecom/test_bed/datalogger.h>
#include "LeakTestTransition.h"

EXPORT_C CLeakTestTransition::CLeakTestTransition(const TDesC&			aTransitionId, 
												  CUnitTestContext&		aUTContext,
												  TTransitionValidator&	aValidator) 
:	CTransition(aTransitionId, aUTContext, aValidator), 
	iFailStep(KMemoryLeakTestFailInit), 
	iBreakStep(KMemoryLeakTestBreakNone)
	{
	// Do nothing here
	}

// Define the overloaded RunL behaviour here
EXPORT_C void CLeakTestTransition::RunL()
	{
	// Setup leak check and call the base RunL
	iThread.HandleCount(iStartProcessHandleCount, iStartThreadHandleCount);
	__UHEAP_SETFAIL(RHeap::EDeterministic,iFailStep);
	__UHEAP_MARK;
	if(iBreakStep == iFailStep)
		{
		// Drop into the debugger because an unexpected leave occured
		// on the last run of the RunL. (This is therefore a repeat run...)
		__DEBUGGER();
		iBreakStep = KMemoryLeakTestBreakNone;
		}

    if(iStatus == KErrNoMemory) 
        { 
        // Special case of Async Process signalling 
        // a failure during the notification 
        _LIT(KAsyncProblem, "CLeakTestTransition::RunL() async completion with error %d."); 
        iUTContext.DataLogger().LogInformationWithParameters(KAsyncProblem, iStatus.Int()); 
        User::Leave(iStatus.Int()); 
        }

	CTransition::RunL();
	if(iTransitionFinished)
		{
		// Successful completion.
		if(iStatus == KErrNone)
			{
			_LIT(KTransitionRunSuccess, "CLeakTestTransition::TransitMethodL() successful completion on iteration %d.");
			iUTContext.DataLogger().LogInformationWithParameters(KTransitionRunSuccess, iFailStep);
			}
		else if(iStatus == KRequestPending)
			{
			//This transition was to set up an asynchronous request
			_LIT(KTransitionRunAsync, "CLeakTestTransition::TransitMethodL() successful called async method on iteration %d.");
			iUTContext.DataLogger().LogInformationWithParameters(KTransitionRunAsync, iFailStep);
			}
		else
			{
			_LIT(KTransitionRunError, "CLeakTestTransition::TransitMethodL() error %d completion on iteration %d.");
			iUTContext.DataLogger().LogInformationWithParameters(KTransitionRunError, iStatus.Int(), iFailStep);
			}
		}
	else
		{
		__DEBUGGER();		// An impossible case ????
		}
	}

EXPORT_C TInt CLeakTestTransition::RunError(TInt aErrorCode)
	{
	if(aErrorCode != KErrNoMemory)
		{
		iLeaveError = aErrorCode;
		// Check if the leave is associated with a repeat request
		// I.e. it was an execution path test from a stub.
		if(iLeaveError == KTestBedRepeatTest && iRepeatThis)
			{
			_LIT(KTransitionRunRepeat, "CLeakTestTransition::TransitMethodL() leaving on iteration %d for repeat test.");
			iUTContext.DataLogger().LogInformationWithParameters(KTransitionRunRepeat, iFailStep);
			User::RequestComplete(iUnitTestStatus, KTestBedRepeatTest);
			}
		else
			{
			iBreakStep = iFailStep;				// Unexpected so
			// Record the leave and signal completed with a leave code
			_LIT(KTransitionRunError, "CLeakTestTransition::TransitMethodL() leaving error %d on iteration %d.");
			iUTContext.DataLogger().LogInformationWithParameters(KTransitionRunError, 
																 aErrorCode, 
																 iFailStep);
			User::RequestComplete(iUnitTestStatus, KTestBedTestLeft);
			}
		}
	else
		{
		++iFailStep;					// Caused by this test harness
		// Safety check in case we are testing a function which always leaves with KErrNoMemory
		if(iFailStep > KProbablyInfinitelyLooping)
			User::RequestComplete(iUnitTestStatus, KTestBedLeakTestLoopDetected);
		else
			{
			// Re-Schedule
			// Only Reset if its not a stub repeat 
			// request via a leave
			if(aErrorCode != KTestBedRepeatTest)
				iTransitionInfo.iIteration = 0;	
			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		}
	// Check leak cleanup
	iThread.HandleCount(iEndProcessHandleCount, iEndThreadHandleCount);
	if(iStartThreadHandleCount != iEndThreadHandleCount)
		{
		__DEBUGGER();										// Oops leaked some handles
		}
	__UHEAP_MARKEND;
	__UHEAP_SETFAIL(RHeap::ENone, KMemoryLeakTestFailInit);	// No more fails
	return KErrNone;
	}

EXPORT_C void CLeakTestTransition::PostTransitionCleanup()
	{
	__UHEAP_SETFAIL(RHeap::ENone, KMemoryLeakTestFailInit);	// No more fails
	}
