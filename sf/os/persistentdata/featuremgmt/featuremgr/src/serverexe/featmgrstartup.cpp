// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "featmgrserver.h"
#include "featmgrdebug.h"
#include "featmgrconfiguration.h"

// -----------------------------------------------------------------------------
// Function that starts the FeatMgrServer.
// -----------------------------------------------------------------------------
//
static void RunServerL()
    {
    FUNC_LOG

    // Naming the server thread after the startup helps to debug panics
    User::LeaveIfError( User::RenameProcess( KServerProcessName ) );
    
    User::LeaveIfError( User::RenameThread( KServerProcessName ) );
     
    // Create and install the active scheduler we need
    CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    
    CActiveScheduler::Install( scheduler );
    
    // Now we are ready to instantiate the actual CServer2 instance
    CFeatMgrServer* server = CFeatMgrServer::NewLC( KServerCActivePriority );

	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);

    INFO_LOG( "RunServerL() - Starting scheduler..." );

	// Ready to run
	CActiveScheduler::Start();

    INFO_LOG( "RunServerL() - Scheduler stopped" );

	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy( server );
	CleanupStack::PopAndDestroy( scheduler );
    }

// -----------------------------------------------------------------------------
// Main function
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    FUNC_LOG

	__UHEAP_MARK;

	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt ret = KErrNoMemory;

	if ( cleanup )
		{
		TRAP( ret, RunServerL() );
		delete cleanup;
		}

	__UHEAP_MARKEND;

	return ret;
    }
