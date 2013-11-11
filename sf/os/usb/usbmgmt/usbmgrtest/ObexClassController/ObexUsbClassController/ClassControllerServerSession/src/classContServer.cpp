/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <e32debug.h>

#include "../../public/clientServerShared.h"
#include "classContServer.h"
#include "classContServerSession.h"

/**
 * Creates a new instance of CClassContServer, a CServer2
 * derived class.
 */
CClassContServer* CClassContServer::NewLC()
	{
	CClassContServer* s = new(ELeave) CClassContServer;
	CleanupStack::PushL(s);
	s->ConstructL();
	s->StartL(KServerName);
	return s;	
	}
/**
 * Destructor
 */
CClassContServer::~CClassContServer()
	{
	delete iShutdownTimer;
	}

/**
 * Constructor
 */	
CClassContServer::CClassContServer() : CServer2(EPriorityStandard)
	{
	}


/**
 * 2nd phase construction. This exists to create a new timer and start a 5 second 
 * timing period.
 */	
void CClassContServer::ConstructL()
	{
	iShutdownTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	StartShutdownTimer();	
	}


/**
 * This function is called by the session object when a session has been terminated.
 * It creates a new timer of 5 seconds, this time the 5 seconds will run to 
 * completion and the timers RunL function will be called, stopping the active scheduler.
 */	

void CClassContServer::SessionRemoved()
	{
	iSession = NULL;
	StartShutdownTimer();
	}


/**
 * Function to instansiate a new CSession2 object, function leaves if a session
 * already exists. After a session has been created the CServerTimer object
 * gets cancelled.
 */	
CSession2* CClassContServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& aMessage) const
	{
	// before we continue check the secure ID of the send of aMessage
	if (aMessage.SecureId() != KUsbSvrSecureId)
		User::Leave(KErrPermissionDenied);

	CClassContServer* ncthis = const_cast<CClassContServer*>(this);
	// Only allow one session
	if (iSession)
		User::Leave(KErrAlreadyExists);	
	ncthis->iSession = CClassContServerSession::NewL(*ncthis);
	ncthis->CancelShutdownTimer();
	return iSession;
	}



	
void CClassContServer::StartShutdownTimer()
	{
	if ( !iShutdownTimer->IsActive() )
		{
		// Start the timer, delay events for 5 seconds, subsequent
		// firings will not occur.
		iShutdownTimer->Start(KShutdownDelay, KShutdownInterval, 
								TCallBack(CClassContServer::ShutdownTimerFired, this));
		}	
	}
	
void CClassContServer::CancelShutdownTimer()
	{
	iShutdownTimer->Cancel();
	}
	
TInt CClassContServer::ShutdownTimerFired(TAny* /*aThis*/)
	{
	CActiveScheduler::Stop();
	return KErrNone;
	}
