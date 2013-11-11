/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements CTrebleRequest.  See class and function definitions for information.
*
*/


/**
 @file
*/

#include "scstestserver.h"


CTrebleRequest* CTrebleRequest::NewL(CScsTestSession* aSession, CScsTestSubsession* aSubsession, const RMessage2& aMessage)
/**
	Factory function allocates new instance of CTrebleRequest which and queues a timer.
	When the timer expires it trebles the value of an integer in client space to complete
	the request later.

	@param	aSession		Session on which this request was launched.
	@param	aSubsession		Subsession on which this request was launched.
	@param	aMessage		Standard server-side handle to message.
 */
	{
	CTrebleRequest* self = new(ELeave) CTrebleRequest(aSession, aSubsession, aMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTrebleRequest::CTrebleRequest(CScsTestSession* aSession, CScsTestSubsession* aSubsession, const RMessage2& aMessage)
/**
	This private constructor is defined to initialize the CAsyncRequest base class with
	the supplied arguments.

	@param	aSession		Session on which this request was launched.
	@param	aSubsession		Subsession on which this request was launched.
	@param	aMessage		Standard server-side handle to message.
 */
:	CAsyncRequest(aSession, aSubsession, aMessage)
	{
	// empty.
	}

void CTrebleRequest::ConstructL()
/**
	Second-phase constructor initializes and launches the timer.
 */
	{
	TInt r = iTimer.CreateLocal();
	User::LeaveIfError(r);
	
	CAsyncRequest::TransferToScsFrameworkL();
	
	iTimer.After(iStatus, ScsTestImpl::KTrebleTimerDelayUs);
	SetActive();
	}

CTrebleRequest::~CTrebleRequest()
/**
	Close the timer which this object used to create a delay.
 */
	{
	iTimer.Close();
	}

void CTrebleRequest::RunL()
/**
	Treble the value passed by the client and complete
	the request.
 */
	{
	// if these descriptor functions leave then RunError
	// will be called by the active object framework.
	TPckgBuf<TInt> valBuf;
	iMessagePtr2.ReadL(0, valBuf);
	valBuf() *= 3;
	iMessagePtr2.WriteL(0, valBuf);
	
	//	CompleteAndMarkForDeletion(KErrNone);
	// Call base class to call CompleteAndMarkForDeletion as this
	// improves code coverage
	CAsyncRequest::RunL();
	}

void CTrebleRequest::DoCancel()
/**
	Implement CActive by cancelling the outstanding activity.
	This does not complete the client request or mark this object
	for deletion.
 */
	{
	iTimer.Cancel();
	}
