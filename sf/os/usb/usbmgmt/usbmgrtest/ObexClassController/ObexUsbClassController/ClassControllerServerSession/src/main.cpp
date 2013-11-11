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

#include <e32base.h>
#include <e32debug.h>

#include "../../public/clientServerShared.h"
#include "classContServer.h"

static void RunServerL();


/**
 *This is the code entry point, calls run server function
 */
GLDEF_C TInt E32Main()
	{

	TInt ret = KErrNoMemory;

	__UHEAP_MARK;

	CTrapCleanup* cleanup = CTrapCleanup::New();
	
	if ( cleanup )
		{
		TRAP(ret, RunServerL());
		delete cleanup;
		}
		
	__UHEAP_MARKEND;
	
	return ret;
	
	}

/**
 * Function to initialise a CServer2 derived class and connect to
 * the client.
 */
static void RunServerL()
	{
	static_cast<void>(User::RenameThread(KServerName));

	// Create and install the active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	(void)CClassContServer::NewLC();

	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);
	// Ready to run. This only returns when the server is closing down.
	CActiveScheduler::Start();
	// Clean up the server and scheduler.
	CleanupStack::PopAndDestroy(2, scheduler);
	}
