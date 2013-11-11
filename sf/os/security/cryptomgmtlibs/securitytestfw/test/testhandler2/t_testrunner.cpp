/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "t_testrunner.h"
#include "t_testaction.h"
#include "t_output.h"

// CTestRunner /////////////////////////////////////////////////////////////////

EXPORT_C CTestRunner::CTestRunner(Output& aOut) :
    CActive(EPriorityNormal),
    iOut(aOut)
    {
	CActiveScheduler::Add(this);
    }

EXPORT_C CTestRunner::~CTestRunner()
    {
    Cancel();
    }

EXPORT_C TInt CTestRunner::PerformPrerequisiteL(CTestAction* aAction)
    {
    TInt err = KErrNone;
    while (!aAction->Finished() && aAction->iActionState == CTestAction::EPrerequisite)
        {
        err = RunAsyncMethodL(&CTestAction::PerformPrerequisite, aAction, err);
        }
    return err;
    }

EXPORT_C TInt CTestRunner::PerformActionL(CTestAction* aAction)
    {
    TInt err = KErrNone;
    while (!aAction->Finished() && aAction->iActionState == CTestAction::EAction)
        {
        err = RunAsyncMethodL(&CTestAction::PerformAction, aAction, err);
        }
    return err;
    }

EXPORT_C TInt CTestRunner::PerformPostrequisiteL(CTestAction* aAction, TInt aInitialStatus)
    {
    TInt err = aInitialStatus;
    while (!aAction->Finished() && aAction->iActionState == CTestAction::EPostrequisite)
        {
        err = RunAsyncMethodL(&CTestAction::PerformPostrequisite, aAction, err);
        }
    return err;
    }

TInt CTestRunner::RunAsyncMethodL(TTestMethod aMethod, CTestAction* aAction, TInt aInitialStatus)
    {
    iStatus = aInitialStatus;
    TRAPD(err, (aAction->*aMethod)(iStatus));
    if (err != KErrNone)
        {
        iStatus = err;
        if (err != KErrNoMemory)
        	{
        	aAction->iActionState = CTestAction::EPostrequisite;
        	}
        }
    else
        {
        SetActive();
		RunSchedulerL();
        }

    return iStatus.Int();
    }

void CTestRunner::RunSchedulerL()
	{
	iSchedulerRunning = ETrue;
	CActiveScheduler::Start();
	}

TBool CTestRunner::StepScheduler()
	{
    User::WaitForAnyRequest();
	TInt err;
    if (!CActiveScheduler::Current()->RunIfReady(err, EPriorityNull)) 
        {
        User::Invariant(); 
        }
	return !IsActive();
	}

EXPORT_C void CTestRunner::RunL()
    {
	if (iSchedulerRunning)
		{
		iSchedulerRunning = EFalse;
		CActiveScheduler::Stop();    
		}
	}

EXPORT_C TInt CTestRunner::RunError(TInt /*aError*/)
    {
    return KErrGeneral; // RunL() can never leave
    }

EXPORT_C void CTestRunner::DoCancel()
    {
    }

// COOMTestRunnerBase ////////////////////////////////////////////////////////////////////

/// Max OOM fail count, to prevent runaway tests
const TInt KOOMFailLimit = 10000;

EXPORT_C COOMTestRunnerBase::COOMTestRunnerBase(Output& aOut) :
    CTestRunner(aOut)
    {
    }

EXPORT_C COOMTestRunnerBase::~COOMTestRunnerBase()
    {
    }

EXPORT_C TInt COOMTestRunnerBase::PerformActionL(CTestAction* aAction)
    {
    iOut.writeString(_L("Running OOM test..."));
    iOut.writeNewLine();            
    iOut.writeString(_L("Fail point:  Heap used:  Action state:  Status:"));
    iOut.writeNewLine();            

	StartOOMTestL();
	
    TInt allocStart = AllocCount();

	TInt failCount;
	TInt err = KErrNone;
	for (failCount = 1 ; failCount < KOOMFailLimit ; ++failCount)
        {
		IncHeapFailPoint();
        
        err = KErrNone;
        TInt actionState = 0;
        while (!aAction->Finished() && aAction->iActionState == CTestAction::EAction && err != KErrNoMemory)
            {
            ++actionState;
            err = RunAsyncMethodL(&CTestAction::PerformAction, aAction, err);            
            }

        TInt allocEnd = AllocCount();
		ResetHeapFail();

        TBuf<128> buffer;
        buffer.Format(_L("  %8d    %8d       %8d %8d"), failCount, allocEnd - allocStart, actionState, err);
        iOut.writeString(buffer);
        iOut.writeNewLine();
        
		if (err != KErrNoMemory || aAction->Finished() || aAction->iActionState != CTestAction::EAction)
            {
			// Test finished
			break;
            }

        aAction->AfterOOMFailure();
        aAction->Reset();
        aAction->ResetState();
        }

	EndOOMTestL();

	if (failCount == KOOMFailLimit)
		{
		// Runaway OOM test
		iOut.writeString(_L("OOM test failed to terminate"));
		iOut.writeNewLine();
		return KErrGeneral;
		}

	return err;
    }

// COOMTestRunner ////////////////////////////////////////////////////////////////////////

COOMTestRunner::COOMTestRunner(Output& aOut) :
    COOMTestRunnerBase(aOut)
    {
    }

COOMTestRunner::~COOMTestRunner()
    {
    }

void COOMTestRunner::StartOOMTestL()
	{
	iFailPoint = 0;
	}

void COOMTestRunner::IncHeapFailPoint()
	{
	++iFailPoint;
	__UHEAP_SETFAIL(RHeap::EDeterministic, iFailPoint);
	}

void COOMTestRunner::ResetHeapFail()
	{
	__UHEAP_RESET;        		
	}

TInt COOMTestRunner::AllocCount()
	{
	return User::CountAllocCells();
	}

void COOMTestRunner::EndOOMTestL()
	{
	}

// CCancelTestRunner /////////////////////////////////////////////////////////////////////

/// Max cancel step, to prevent runaway tests
const TInt KCancelStepLimit = 200;

CCancelTestRunner::CCancelTestRunner(Output& aOut) :
    CTestRunner(aOut)
    {
    }

CCancelTestRunner::~CCancelTestRunner()
    {
    }

/**
 * Run the async PerformAction method for a specified number of steps and then
 * cancel it.  This does the equivalent of RunAsyncMethod, but calling
 * PerformAction and cancelling it.
 */
TInt CCancelTestRunner::RunAndCancelPeformActionMethod(CTestAction* aAction, TInt aInitialStatus,
                                                       TInt aCancelStep, TInt& aStep)
    {
    iStatus = aInitialStatus;
    TRAPD(err, aAction->PerformAction(iStatus));
    if (err != KErrNone)
        {
        return err;
        }

    SetActive();
    
    // This is our equivalent of an active scheduler loop
	while (IsActive())
		{
		StepScheduler();
		
        // Check if we can cancel this step
        if (iStatus.Int() == KRequestPending)
			{
            ++aStep;
			// Check if this is the step we want to cancel
			if (aStep == aCancelStep)
				{
				// Cancel request
				aAction->PerformCancel();

				// Check request completed immediately
				if (iStatus.Int() == KRequestPending)
					{
					iOut.writeString(_L("Cancelled request not completed immediately!"));
					iOut.writeNewLine();
					iAbort = ETrue;
					}
				}
			}
		}

    return iAbort ? KErrGeneral : iStatus.Int();
    }

/**
 * Run the test action for a specified number of steps and then cancel it.
 */
TInt CCancelTestRunner::RunAndCancelTestAction(CTestAction* aAction, TInt aCancelStep)
    {
    TInt err = KErrNone;
	TInt step = 0;
    TInt actionState = 0;
    while (!iAbort && !aAction->Finished() && aAction->iActionState == CTestAction::EAction && err != KErrCancel)
        {
        ++actionState;
		err = RunAndCancelPeformActionMethod(aAction, err, aCancelStep, step);
        }

	TBuf<128> buffer;
	buffer.Format(_L(" %8d      %8d      %8d %8d"), aCancelStep, step, actionState, err);
	iOut.writeString(buffer);
	iOut.writeNewLine();

    return err;
    }

TInt CCancelTestRunner::PerformActionL(CTestAction* aAction)
    {
    iOut.writeString(_L("Running cancellation test..."));
    iOut.writeNewLine();            
    iOut.writeString(_L("Fail step:  Total steps:  Action state:  Status:"));
    iOut.writeNewLine();            

    iAbort = EFalse;
    for (TInt step = 1 ; step <= KCancelStepLimit ; ++step) 
        {
        TInt err = RunAndCancelTestAction(aAction, step);

        if (iAbort || aAction->Finished() || aAction->iActionState != CTestAction::EAction)	
			{
            return err;
			}
		
        aAction->Reset();
        }
    
    // Runaway cancel test
    iOut.writeString(_L("Cancel test failed to terminate"));
    iOut.writeNewLine();
    return KErrGeneral;
    }
