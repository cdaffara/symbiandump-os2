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
// Server startup code and session object implementation.
// 
//

#include "EComDebug.h"
#include "EComServerStart.h"
#include "EComServer.h"

// __________________________________________________________________________
// Server startup code
static void RunServerL()
//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
	{
	// naming the server thread after the server helps to debug panics
    User::LeaveIfError(User::RenameThread(KEComServerName));
	//
	// create and install the active scheduler we need
	CActiveScheduler* scheduler=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	//
	// create the server (leave it on the cleanup stack)
	CEComServer::NewLC();
	//
	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);
	//
	// Ready to run
	__ECOM_TRACE("ECOM: Server ready, starting active scheduler");
	CActiveScheduler::Start();
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2, scheduler);
	}



TInt ServerStart()
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt err=KErrNoMemory;
	if (cleanup)
		{
		TRAP(err,RunServerL());
		if (err != KErrNone)
            {
            __ECOM_LOG1("ECOM: Server unexpectedly exited, error = %d", err);
            }
		delete cleanup;
		}
	return err;	
	}

//
// Main entry-point for the server thread
//
#ifndef __ECOMSERVER_TESTING__
TInt E32Main()
//
// Server process entry-point
// Recover the startup parameters and run the server
//
	{
	__UHEAP_MARK;
	__ECOM_TRACE("ECOM: Server started, initialisation begun");
	
	TInt err = ServerStart();

	__UHEAP_MARKEND;
	return err;
	}
#endif	// __ECOMSERVER_TESTING__
