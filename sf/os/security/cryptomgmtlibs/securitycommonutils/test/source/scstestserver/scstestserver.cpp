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
* Implements CScsTestServer.  See class and function definitions
* for more information.
*
*/


/**
 @file
*/
#include <f32file.h>
#include "scstestserver.h"
#include "scstestclient.h"


CScsTestServer* CScsTestServer::NewLC()
/**
	Factory function allocates a new, initialized instance of CScsTestServer.

	@return					New instance of CScsTestServer, which is owned by the
							caller and left on the cleanup stack.  On return, the
							server has been started.
 */
	{
	CScsTestServer* self = new(ELeave) CScsTestServer();
	CleanupStack::PushL(self);
	RFs fs;
	User::LeaveIfError(fs.Connect());
	TUint attr;
	TInt r = fs.Att(KDisableScsTestServerTimeout(), attr);
	if(r == KErrNone)
		{
		self->ConstructL(0);
		}
	else
		{
		self->ConstructL(ScsTestImpl::KShutdownPeriodUs);
		}
	return self;
	}

CScsTestServer::CScsTestServer()
/**
	This private constructor prevents direct instantiation.
 */
 :	CScsServer(ScsTestImpl::Version())
	{
	// empty.
	}

void CScsTestServer::ConstructL(TInt aShutdownPeriodUs)
/**
	Secondary initialization initializes the base class and
	starts the server.
 */
	{
	CScsServer::ConstructL(aShutdownPeriodUs);
	StartL(ScsTestImpl::KServerName);
	}

CScsTestServer::~CScsTestServer()
/**
	This empty constructor provides a single point of definition.
 */
	{
	// empty.
	}

CScsSession* CScsTestServer::DoNewSessionL(const RMessage2& aMessage)
/**
	Implement CSessionCountServer by allocating a new instance of CScsSession.

	@param	aMessage		Connect message.  Not used.
	@return					New, initialized instance of CScsTestSession.
							This object will be destroyed by the framework
							when the session is closed.
 */
	{
	(void) aMessage;

	return CScsTestSession::NewL(*this);
	}

// End of file
