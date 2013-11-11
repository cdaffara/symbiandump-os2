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
* Starts up server and exits the process when the server has stopped.
*
*/


#include "sraserver.h"
#include <e32debug.h>


static void RunServerL()
/**
	Allocate and start the streaming reference agent server.
 */
	{
	// First create and install the active scheduler
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// create the server
	(void) CSraServer::NewLC();
	
	// Naming the server thread after the server helps to debug panics
	User::LeaveIfError(User::RenameThread(KSraName));
	
	// tell launching process the server has started successfully
	RProcess::Rendezvous(KErrNone);

	CActiveScheduler::Start();

	// the active scheduler has been stopped here because there have
	// been no current sessions for the shutdown period.
	CleanupStack::PopAndDestroy(2, scheduler);
	}


GLDEF_C TInt E32Main()
/**
	Server process entrypoint. Calls RunServerL function which starts 
	the server and does not return until the server shuts down.

	@return	Symbian OS error code where KErrNone indicates success 
			and any other value indicates failure.
 */
	{
	__UHEAP_MARK;

	// allocating a cleanup stack also installs it
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt r = KErrNoMemory;
	if (cleanup)
		{
		TRAP(r, RunServerL());
		delete cleanup;
		}

	__UHEAP_MARKEND;

	return r;
	}
