/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


/**
 @file
 @test
 @internalComponent Internal Symbian test code
*/

#include "FNTSTORE.H"
#include "OPENFONT.H"
#include "FNTBODY.H"
#include "FNTSTD.H"
#include "t_cachedeletion.h"
#include <hal.h>
#include <s32file.h>
#include <graphics/shapeimpl.h>

_LIT(KSecondProcess,"tcachedeletionprocess");
_LIT(KThirdProcess,"tcachedeletionprocess");

class CTCacheDeletion : public CTGraphicsBase
	{
public:
	CTCacheDeletion(CTestStep* aStep);
	~CTCacheDeletion();
protected:
// From CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void RunProcess2L();
	};

CTCacheDeletion::CTCacheDeletion(CTestStep* aStep)
 :	CTGraphicsBase(aStep)
	{

	}

CTCacheDeletion::~CTCacheDeletion()
	{
	// no action needed
	}

void TimerCleanup(TAny *aTimer)
	{
	((RTimer*)aTimer)->Cancel();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0050

	@SYMTestCaseDesc
	Tests font and bitmap server disconnection (when a process finishes)
	at the same time a process is outputing.

	@SYMTestActions
	This function creates two processes.  These processes are called
    process 3 and process 2.  Process 3 is created, and run.  There are
    parameters input to the process.  The first is an indicator to the
    process of the instance (2 or 3) and the second is a timeout.
    Process3 runs continually, drawing text for the full timeout period.
    Process2 is created and allowed to finish in a loop in this function for
    the same timeout period, currently ten seconds.

	@SYMTestExpectedResults
	Test should pass
*/
void CTCacheDeletion::RunProcess2L()
	{
	TInt runForMicroSecs = 1000*1000*10;

	//create process 3
	RProcess process3;
	TInt err;
	err = process3.Create(KThirdProcess, KNullDesC);
	User::LeaveIfError(err);
	CleanupClosePushL(process3);

	//run process 3
	process3.SetParameter(1,1);
	process3.SetParameter(2,runForMicroSecs);
	TRequestStatus completeStatus3;
	process3.Logon(completeStatus3);
	TRequestStatus rendezvousStatus3;
	rendezvousStatus3=KRequestPending;
	process3.Rendezvous(rendezvousStatus3);
	process3.Resume(); //start the process
	User::WaitForRequest(rendezvousStatus3);
	
	//create a timer
	RTimer timer;
	timer.CreateLocal();
	TRequestStatus timerStatus=KRequestPending;
	TTimeIntervalMicroSeconds32 timeout(runForMicroSecs);
	timer.After(timerStatus, runForMicroSecs);

	TCleanupItem cleanup(TimerCleanup,&timer);
	CleanupStack::PushL(cleanup);

	do
		{
		//run process 2 in a loop

		//create process 2
		RProcess process2;
		TInt err = process2.Create(KSecondProcess, KNullDesC);
		User::LeaveIfError(err);
		CleanupClosePushL(process2);

		//start process 2
		process2.SetParameter(1,0);
		process2.SetParameter(2,runForMicroSecs);//not actually used
		TRequestStatus completeStatus2;
		process2.Logon(completeStatus2);
		TRequestStatus rendezvousStatus2;
		rendezvousStatus2=KRequestPending;
		process2.Rendezvous(rendezvousStatus2);
		process2.Resume(); //start the process

		//rendezvous with 2
		User::WaitForRequest(rendezvousStatus2);
		TInt status = rendezvousStatus2.Int();
		TEST(status ==1);

		//now let 2 finish
		User::WaitForRequest(completeStatus2);
		TExitType exit = process2.ExitType();
		TEST(exit == EExitKill);
		TInt reason = process2.ExitReason();
		TEST (reason == 0);

		CleanupStack::PopAndDestroy();
		}
	while (timerStatus==KRequestPending);
	timer.Cancel();
	CleanupStack::Pop();//timer

	//rendezvous with 3
	User::WaitForRequest(rendezvousStatus3);
	TInt status = rendezvousStatus3.Int();
	TEST(status ==10);

	//now let 3 finish
	User::WaitForRequest(completeStatus3);
	TExitType exit = process3.ExitType();
	TEST(exit == EExitKill);
	TInt reason = process3.ExitReason();
	TEST (reason == 0);

	CleanupStack::PopAndDestroy();//process 3 
	}
	
void CTCacheDeletion::RunTestCaseL( TInt aCurTestCase )
	{
#if defined __WINS__ || defined __WINSCW__
	aCurTestCase = aCurTestCase;  //to avoid unused warning
	TestComplete(); //only run test on hardware, always passes on winscw
	return;
#endif
	((CTCacheDeletionStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	switch ( aCurTestCase )
		{
	case 1:	
		((CTCacheDeletionStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0050"));
		RunProcess2L();
		break;
	case 2:
		((CTCacheDeletionStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTCacheDeletionStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTCacheDeletionStep*)iStep)->RecordTestResultL();
	}

// --------------
__CONSTRUCT_STEP__(CacheDeletion)
