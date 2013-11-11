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
//

#include <e32debug.h>
#include "testscheduler.h"

///////////////////////////////////////////////////////////////////////////////////////
// Member Functions
///////////////////////////////////////////////////////////////////////////////////////

/**
	Constructor.
	C++ default constructor can NOT contain any code, that might leave.
*/
CTestScheduler::CTestScheduler()
	{

	}

/**
	Destructor.
*/
CTestScheduler::~CTestScheduler()
	{
	delete iControl;
	}

/**
	Two-phase constructor.
*/
CTestScheduler* CTestScheduler::NewL()
	{

	CTestScheduler* self = new( ELeave )CTestScheduler();
	CleanupStack::PushL( self );

	if (CActiveScheduler::Current() == NULL)
	{
		CActiveScheduler::Install(self);
	}

	self->ConstructL();
	CleanupStack::Pop( self );

	return self;

	}

/**
	Symbian 2nd phase constructor can leave.
*/
void CTestScheduler::ConstructL()
	{
	iControl = new(ELeave) CControlObject;
	}

/**
	Waits for any request from user
*/
void CTestScheduler::WaitForAnyRequest()
	{
	User::WaitForAnyRequest();
	}


/**
	Handles the result of a leave occurring in an active object's RunL() function.
	
	@param  TInt anError: error reason
*/
void CTestScheduler::Error(TInt /*anError*/) const
	{
	Stop();
	}


/**
	Start scheduler for given amount of time
	
	@param  TInt aMillisecs: time to run the scheduler 
*/
TBool CTestScheduler::StartAndWaitL
		(
		TInt aMillisecs //timeout
		)
	{
	//initialize the control object with a timer and start scheduler
	CTestScheduler* self = CTestScheduler::NewL();

	TBool ret = ETrue;
	self->iControl->Start(aMillisecs);

	CActiveScheduler::Start();

	// if not timed, then timer must be cancelled
	if (!self->iControl->Timed())
	{
		self->iControl->Cancel();
	}

	delete self;

	return ret;
	}


///////////////////////////////////////////////////////////////////////////////////////
// CControlObject
///////////////////////////////////////////////////////////////////////////////////////

/**
	Constructor.
	C++ default constructor can NOT contain any code, that might leave.
*/
CControlObject::CControlObject()
	:CActive(0)
	{
	iTimer.CreateLocal();
	iState = EStateStarting;
	iTimeout = 1000;

	CActiveScheduler::Add(this);			
	}

/**
	Destructor.
*/
CControlObject::~CControlObject()
	{
	Cancel();
	iTimer.Close();
	}

/**
	Starts waiting.
	
	@param  TInt aTimeoutInMilliSecs: in: timeout in milliseconds
*/
void CControlObject::Start(TInt aTimeoutInMilliSecs)
	{
	iState = EStateStarting;
	iTimeout = aTimeoutInMilliSecs;
	// The object is ready to run
	TRequestStatus* p = &iStatus;
	User::RequestComplete(p, KErrNone);
	SetActive();
	}

/**
	Cancels waiting.
*/
void CControlObject::DoCancel()
	{
	iTimer.Cancel();
	iState = EStateFinished;
	}

/**
	Switch to the next state
*/
void CControlObject::RunL()
	{
	switch (iState)
		{
		case EStateStarting:
			{
			RDebug::Print(_L("CControlObject::RunL - Starting\n"));
			// Start the timeout timer and switch to the next state
			iTimer.After(iStatus, 1000 * iTimeout);
			iState = EStateStarted;
			SetActive();

			// Signal that we are running
			RThread().Rendezvous(KErrNone);
			}
			break;

		case EStateStarted:
			{
			RDebug::Print(_L("CControlObject::RunL - Time is out\n"));
			// Time is out
			iState = EStateTimed;
			CActiveScheduler::Stop();
			break;
			}

		default: // just in case - we are not supposed to get here
			{
			iState = EStateFinished;
			CActiveScheduler::Stop();
			break;
			}
		}
	}
