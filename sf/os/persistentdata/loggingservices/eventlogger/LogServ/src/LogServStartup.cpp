// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "LogServShared.h"
#include "LogServServer.h"
#include "LogServScheduler.h"
#include "logservpanic.h"

static void StartLogServerL()
//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
	{
#if defined(LOGGING_CREATE_LOG_DIRECTORY) && defined(LOGGING_ENABLED)
	RFs fsSession;
	if	(fsSession.Connect() == KErrNone)
		{
		_LIT(KBaseFolder, "_:\\Logs\\");
		TFileName path(KBaseFolder);
		path[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive()); 
		path += KLogFolder;
		path.Append(KPathDelimiter);
		fsSession.MkDirAll(path); // ignore error
		}
	fsSession.Close();

	//LOGNEW;

    // For !WINS or all EKA2 builds this is needed
 	// Write the process location
	TFileName processName(RProcess().FileName());
	LOGTEXT2("StartLogServerL(%S)", &processName);

#endif

	// Naming the server thread after the server helps to debug panics
	LOGTEXT("StartLogServerL() - about to rename thread");
    User::LeaveIfError(User::RenameThread(KLogServerName));

	// Create and install the active scheduler we need
	CActiveScheduler* scheduler = new(ELeave)CLogActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// create the server (leave it on the cleanup stack)
	LOGTEXT("StartLogServerL() - about to create server object");
	CLogServServer::NewLC();

	// Initialisation complete, now signal the client
	LOGTEXT("StartLogServerL() - about to signal client thread");
	RProcess::Rendezvous(KErrNone);
	// Start the scheduler and wait for clien requests
	LOGTEXT("StartLogServerL() - about to start scheduler");
	CActiveScheduler::Start();

	// Cleanup the server and scheduler
	LOGTEXT("StartLogServerL() - scheduler stopped, exiting main log engine server thread");
	CleanupStack::PopAndDestroy(2, scheduler);
	}

TInt E32Main()
//
// Server process entry-point
// Recover the startup parameters and run the server
//
	{
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,StartLogServerL());
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	return r;
	}


