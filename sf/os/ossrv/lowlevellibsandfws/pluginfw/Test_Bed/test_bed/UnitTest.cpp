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
// Implementation of the CUnitTest base class.
// 
//

#include <ecom/test_bed/unittest.h>
#include <ecom/test_bed/transition.h>
#include <ecom/test_bed/datalogger.h>
#include <ecom/test_bed/testbeddefinitions.h>


EXPORT_C CUnitTest::~CUnitTest()
	{
	Cancel();

	// Delete any outstanding asynchronous transitions
	if(iOutstandingTransitions)
		{
		iOutstandingTransitions->Reset();
		delete iOutstandingTransitions;
		}

	if(iTransitions)
		{
		iTransitions->ResetAndDestroy();
		delete iTransitions;
		}

	iLeaveErrorArray.Reset();

	delete iFileMan;
	iFs.Close();
	}

EXPORT_C void CUnitTest::UnitTestConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iFileMan = CFileMan::NewL(iFs);
	CTimer::ConstructL();
	iTransitions = new(ELeave) RPointerArray<CTransition>;
	iOutstandingTransitions = new(ELeave) RPointerArray<CTransition>;
	_LIT(KConstructingUnitTestMsg, "Constructed Unit Test named %S");
	iDataLogger.LogInformationWithParameters(KConstructingUnitTestMsg, &iUnitTestName);
	}


CUnitTestInfo* CUnitTest::TransitionSetL() const
	{
	CUnitTestInfo* transitionSet = CUnitTestInfo::NewL(iUnitTestName);
	return transitionSet;
	}


EXPORT_C void CUnitTest::RunTest(TTimeIntervalMicroSeconds32 aTimeAfter /*= 0*/)
	{
	After(aTimeAfter);
	_LIT(KTxtSeparator, "-----------------------------------------------------------------------------------");
	_LIT(KStartingUnitTest, "Beginning Unit Test named %S");
	iDataLogger.LogInformation(KTxtSeparator);
	iDataLogger.LogInformationWithParameters(KStartingUnitTest, &iUnitTestName);
	iDataLogger.ReportInformationWithParameters(KStartingUnitTest, &iUnitTestName);
	}

EXPORT_C void CUnitTest::RunL()
	{
	_LIT(KUnitTestRunLPanic, "CUnitTest::RunL");

	TInt status = iStatus.Int();
	switch(status)
		{
		case (KTestBedRepeatTest):			/* A stub has requested repeat of the last test */
			// Go back one so that we repeat the last test
			--iNextTransitionIndex;
			break;

		case (KTestBedTestLeft):				/* The last transition's RunL left */
		case (KTestBedTestCancel):				/* The last transition was cancelled */
		case (KTestBedLeakTestLoopDetected):	/* A leak test detected an infinite loop */
		case (KTestBedFailedPreConditions):		/* The last transition failed it's pre conditions */
		case (KTestBedFailedPostConditions):	/* The last transition failed it's post conditions */
			// Go to the end of the test so that it finishes
			iNextTransitionIndex = iTransitions->Count();
			break;

		case (KTestBedAsynchronousTransition):	/* The last transition started an async request */
			// Remember that we have an outstanding request and then carry on
			iOutstandingTransitions->Append((*iTransitions)[iNextTransitionIndex - 1]);
			break;

		case (KErrNone):
			break;

		default:
			User::Panic(KUnitTestRunLPanic, KTestBedInvalidStatus);
		}

	// If we still have more transitions to run
	if(iNextTransitionIndex < iTransitions->Count())
		{
		iStatus = KRequestPending;
		SetActive();

		// If the next transition is a blocking one then wait for all outstanding async
		// requests to complete.  Otherwise just run the next transition
		if(((*iTransitions)[iNextTransitionIndex]->IsBlockingTransition()) && 
			(iOutstandingTransitions->Count() > 0))
			{
			iWaitingForCompletion = ETrue;
			}
		else
			{
			(*iTransitions)[iNextTransitionIndex]->RunTransition(&iStatus);
			++iNextTransitionIndex;
			}
		}
	else
		{
		// If we still have outstanding async requests then wait for these to complete
		// otherwise we have finished this test
		if(iOutstandingTransitions->Count() > 0)
			{
			iWaitingForCompletion = ETrue;
			iStatus = KRequestPending;
			SetActive();
			}
		else
			{
			iUnitTestObserver.Complete(this);

			_LIT(KInfoPrintFailed, "Failed: Unit Test");
			_LIT(KTestLeft, "Failed: Unit Test %S left");
			_LIT(KTestLeftWithUnexpectedError, "Failed: Test %S left with unexpected error");
			_LIT(KTestFailed, "Failed: Unit Test %S failed a pre/post condition validation check");
			_LIT(KTestLeftWithExpectedError, "Test %S left with an anticipated error");
			_LIT(KTestCancelled, "Cancelled: Unit Test Transition %S was cancelled");
			_LIT(KTestEnteredInfiniteLoop, "Unit Test Transition %S aborted (infinitely looping)");
			_LIT(KEndingUnitTest, "Successfully completed Unit Test %S");
			// We use RTest if it is present in the framework to validate
			// status codes for errors. Note: not all non KErrNone code mean 
			// there was an error and so we need to selective over which 
			// cases below we use RTest().
			switch(status)
				{
				case (KTestBedTestLeft):
					{
					TInt leaveCode = iCurrentlyExecutingTransition->GetErrorCode();
					//Check to see if the leave code is NOT on the list of known leaving codes
					if(iLeaveErrorArray.Find(leaveCode) == KErrNotFound)
						{
						iDataLogger.LogInformationWithParameters(KTestLeft, &iUnitTestName);
						iDataLogger.ReportInformationWithParameters(KTestLeft, &iUnitTestName);
						if(iRTest) 
							{
							(*iRTest)(status==KErrNone);
							}
						}
					else	//Leave code is on the list 
						{
						TInt count = iTransitions->Count();
						//Check transition number and if it is the last transition then this is an expected error
						CTransition* lastTransition = (*iTransitions)[count-1];
						if(iCurrentlyExecutingTransition == lastTransition)
							{
							iDataLogger.LogInformationWithParameters(KTestLeftWithExpectedError, &iUnitTestName);
							iDataLogger.ReportInformationWithParameters(KEndingUnitTest, &iUnitTestName);
							}
						else	//Otherwise, if not the last transition, the test failed with an unexpected error
							{
							User::InfoPrint(KInfoPrintFailed);
							User::InfoPrint(iUnitTestName);
							iDataLogger.LogInformationWithParameters(KTestLeftWithUnexpectedError, &iUnitTestName);
							iDataLogger.ReportInformationWithParameters(KTestLeftWithUnexpectedError, &iUnitTestName);
							if(iRTest) 
								{
								(*iRTest)(status==KErrNone);
								}
							}
						}
					}
					break;

				case (KTestBedFailedPreConditions):
				case (KTestBedFailedPostConditions):
					{
					User::InfoPrint(KInfoPrintFailed);
					User::InfoPrint(iUnitTestName);
					iDataLogger.LogInformationWithParameters(KTestFailed, &iUnitTestName);
					iDataLogger.ReportInformationWithParameters(KTestFailed, &iUnitTestName);
					if(iRTest) 
						{
						(*iRTest)(status==KErrNone);
						}
					}
					break;

				case (KTestBedTestCancel):
					iDataLogger.LogInformationWithParameters(KTestCancelled, &iUnitTestName);
					iDataLogger.ReportInformationWithParameters(KTestCancelled, &iUnitTestName);
					if(iRTest) 
						{
						(*iRTest)(status==KErrNone);
						}
					break;

				case (KTestBedLeakTestLoopDetected):
					iDataLogger.LogInformationWithParameters(KTestEnteredInfiniteLoop, &iUnitTestName);
					iDataLogger.ReportInformationWithParameters(KTestEnteredInfiniteLoop, &iUnitTestName);
					if(iRTest) 
						{
						(*iRTest)(status==KErrNone);
						}
					break;

				case (KErrNone):
					iDataLogger.LogInformationWithParameters(KEndingUnitTest, &iUnitTestName);
					iDataLogger.ReportInformationWithParameters(KEndingUnitTest, &iUnitTestName);
					break;

				default:
					User::Panic(KUnitTestRunLPanic, KTestBedInvalidStatus);
				}
			}
		}
	}

EXPORT_C void CUnitTest::AddTransitionL(CTransition* aTransition)
	{
	__ASSERT_DEBUG(aTransition, User::Panic(_L("CUnitTest"), KErrTestBedInvalidTransition));
	CleanupStack::PushL(aTransition);
	User::LeaveIfError(iTransitions->Append(aTransition));
	CleanupStack::Pop(aTransition);
	}

EXPORT_C void CUnitTest::AddBlockingTransitionL(CTransition* aTransition)
	{
	__ASSERT_DEBUG(aTransition, User::Panic(_L("CUnitTest"), KErrTestBedInvalidTransition));
	aTransition->SetBlockingTransition(ETrue);
	CleanupStack::PushL(aTransition);
	User::LeaveIfError(iTransitions->Append(aTransition));
	CleanupStack::Pop(aTransition);
	}

EXPORT_C void CUnitTest::AddLeaveErrorCodeL(TInt aLeaveErrorCode)
	{
	User::LeaveIfError(iLeaveErrorArray.Append(aLeaveErrorCode));
	}

EXPORT_C CTransition& CUnitTest::GetCurrentTransition() const
	{
	// Check fror a stray stub call
	// We will always have a valid pointer here IF called
	// from a stub in response to that transition's call 
	// on the stub's methods.
	__ASSERT_DEBUG(iCurrentlyExecutingTransition, User::Invariant());
	return *iCurrentlyExecutingTransition;
	}

EXPORT_C void CUnitTest::SetCurrentTransition(CTransition& aTransition)
	{
	iCurrentlyExecutingTransition = &aTransition;
	}

EXPORT_C void CUnitTest::Complete(CTransition& aTransition, TInt aAsyncPostCheckError)
	{
	// Should never be NULL at this point
	__ASSERT_DEBUG(iCurrentlyExecutingTransition, User::Invariant());
	if(iCurrentlyExecutingTransition == &aTransition)
		iCurrentlyExecutingTransition = NULL;	// Clear the current transition

	// Oops the code will crash if this is ever false
	__ASSERT_DEBUG(iOutstandingTransitions, User::Invariant());

	// Look-up the transition passed in...
	TInt index = iOutstandingTransitions->Find(&aTransition);
	if(index != KErrNotFound)
		{
		// ... and remove from the set of outstanding ones.
		iOutstandingTransitions->Remove(index);

		// Did we fail a second-phase post-condition validation on an asynchronous transition?
		// Or was it a normal transition completion?  Regardless, we go for another iteration 
		// of the AO, passing through the error code.
		TBool completeIt = (aAsyncPostCheckError != KErrNone);
		if(iWaitingForCompletion && (iOutstandingTransitions->Count() == 0))
			completeIt = ETrue;

		if (completeIt)
			{
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, aAsyncPostCheckError);
			}
		}
	else
		{
		__ASSERT_DEBUG(ETrue, 
			User::Panic(_L("CUnitTest"), KErrTestBedInvalidTransition));
		}
	}

EXPORT_C void CUnitTest::SetParametersL(TAny* /*aParams*/)
	{
	// Do nothing
	}

EXPORT_C void CUnitTest::DoCancel()
	{
	CTimer::DoCancel();

	if(iCurrentlyExecutingTransition)
		iCurrentlyExecutingTransition->Cancel();

	// Cancel any outstanding asynchronous transitions
	if(iOutstandingTransitions)
		{
		TInt count = iOutstandingTransitions->Count();
		for(TInt index = 0; index < count; ++index)
			{
			(*iOutstandingTransitions)[index]->Cancel();
			}
		}

	iUnitTestObserver.Complete(this);
	}

