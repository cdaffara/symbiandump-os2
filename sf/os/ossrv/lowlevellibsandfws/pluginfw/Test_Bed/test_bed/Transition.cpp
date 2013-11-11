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

#include <ecom/test_bed/transition.h>
#include <ecom/test_bed/datalogger.h>
#include <ecom/test_bed/testbeddefinitions.h>

EXPORT_C CTransition::CTransition(const TDesC&					aTransitionId, 
										CUnitTestContext&		aUTContext,
										TTransitionValidator&	aValidator) 
:	CActive(CActive::EPriorityStandard),
iTransitionId(aTransitionId),
iUTContext(aUTContext),
iValidator(aValidator),
iTransitionInfo(iTransitionId, iUTContext.DataLogger()) 
	{
	CActiveScheduler::Add(this);
	}


EXPORT_C CTransition::~CTransition()
	{
	Cancel();
	}


EXPORT_C void CTransition::SetStartStateL()
	{
	// Do nothing here
	}


EXPORT_C const TDesC& CTransition::TransitionId() const
	{
	return iTransitionId;
	}

EXPORT_C void CTransition::DoCancel()
	{
	// Complete the unit test so that this test will now finish
	// if the transition has not finished then we haven't even had to chance
	// to run yet so complete the UnitTest with a Cancel message
	// If the transition has finished then we must have an outstanding async
	// transition so Complete the observer to ensure it gets removed from the list
	// of outstanding transitions
	if(!iTransitionFinished)
		User::RequestComplete(iUnitTestStatus, KTestBedTestCancel);
	else
		iUTContext.TransitionObserver().Complete(*this, KTestBedTestCancel);
	}


EXPORT_C void CTransition::RunTransition(TRequestStatus* aUnitTestStatus)
	{
	iUnitTestStatus = aUnitTestStatus;
	iTransitionFinished = EFalse;

	// Reset the iRepeat flag - if this transition should be repeated it will be
	// set during the execution of TransitMethodL().
	iRepeatThis = EFalse;

	if(!iValidator.ValidatePreConditions())
		{
		User::RequestComplete(iUnitTestStatus, KTestBedFailedPreConditions);
		}
	else
		{
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

EXPORT_C void CTransition::RunL()
	{
	iUTContext.TransitionObserver().SetCurrentTransition(*this);
	if(!iTransitionFinished)
		{
		++iTransitionInfo.iIteration;
		TransitMethodL();
		iTransitionFinished = ETrue;
		PostTransitionCleanup();		// Allow the derived transition class to do something before
										// PostCondition validation.

		// If the user called an asynchronous function then SetActive so that we run again
		// when the async function completes
		iAsyncTransition = iStatus == KRequestPending;
		if(iAsyncTransition)
			{
			SetActive();
			}

		TTestBedAsyncState asyncState = iAsyncTransition ? EAsyncCalled : EAsyncCompleted;
		if(!iValidator.ValidatePostConditions(asyncState))
			{
			if(iAsyncTransition)
				{
				//Cancel the request
				Cancel();
				}
			User::RequestComplete(iUnitTestStatus, KTestBedFailedPostConditions);
			}
		else if(iAsyncTransition)
			User::RequestComplete(iUnitTestStatus, KTestBedAsynchronousTransition);
		else if(iRepeatThis)
			User::RequestComplete(iUnitTestStatus, KTestBedRepeatTest);
		else
			User::RequestComplete(iUnitTestStatus, KErrNone);
		}
	else
		{
		if(iAsyncTransition && !iValidator.ValidatePostConditions(EAsyncCompleted))
			iUTContext.TransitionObserver().Complete(*this, KTestBedFailedPostConditions);
		else
			iUTContext.TransitionObserver().Complete(*this, KErrNone);
		}
	}

EXPORT_C const TTransitionInfo& CTransition::TransitionInfo() const
	{
	return iTransitionInfo;
	}

EXPORT_C void CTransition::RepeatOnce()
	{
	iRepeatThis = ETrue;
	}

EXPORT_C TInt CTransition::RunError(TInt aErrorCode)
	{
	// Record the leave and signal completed with a leave code
	_LIT(KTransitionRunError, "CTransition::TransitMethodL() leaving error %d.");
	iUTContext.DataLogger().LogInformationWithParameters(KTransitionRunError, aErrorCode);

	iLeaveError = aErrorCode;
	// Check if the leave is associated with a repeat request
	// I.e. it was an execution path test from a stub.
	if(iLeaveError == KTestBedRepeatTest && iRepeatThis)
		User::RequestComplete(iUnitTestStatus, KTestBedRepeatTest);
	else
		User::RequestComplete(iUnitTestStatus, KTestBedTestLeft);
	return KErrNone;
	}

EXPORT_C TBool CTransition::IsBlockingTransition() const
	{
	return iBlockingTransition;
	}

EXPORT_C void CTransition::SetBlockingTransition(TBool aBlocking)
	{
	iBlockingTransition = aBlocking;
	}

EXPORT_C TInt CTransition::GetErrorCode() const
	{
	return iLeaveError;
	}

EXPORT_C void CTransition::PostTransitionCleanup()
	{
	// Default behaviour is to do nothing
	}
