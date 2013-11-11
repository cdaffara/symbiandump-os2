/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <e32base.h>
#include "fdfserver.h"
#include <usb/usblogger.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mainTraces.h"
#endif


static void RunFdfL();


GLDEF_C TInt E32Main()
	{
    OstTrace0( TRACE_NORMAL, FDF_SERVER_SRC_E32MAIN, ">>E32Main" );

	TInt ret = KErrNoMemory;

	__UHEAP_MARK;

	CTrapCleanup* cleanup = CTrapCleanup::New();

	if ( cleanup )
		{
		// Create the logger object

		TRAP(ret, RunFdfL());


		delete cleanup;
		}

	__UHEAP_MARKEND;

	OstTrace1( TRACE_NORMAL, FDF_SERVER_SRC_E32MAIN_DUP1, 
	        "<<E32Main ret = %d", ret );
	return ret;
	}

static void RunFdfL()
	{
	// Create and install the active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Create the server
	CFdfServer::NewLC();

	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);

	// Ready to run. This only returns when the server is closing down.
	CActiveScheduler::Start();

	// Clean up the server and scheduler.
	CleanupStack::PopAndDestroy(2, scheduler);
	}
