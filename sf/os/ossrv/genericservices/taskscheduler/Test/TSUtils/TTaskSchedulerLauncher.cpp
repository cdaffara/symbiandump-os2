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

#include <e32base.h>
#include <e32debug.h>

/**
 * This function will Start Task Scheduler directly calling Schexe.exe
 * It is used for test performance of Task Scheduler
 */ 	
static void LaunchTaskSchedulerL()
	{

	TRequestStatus stat;
	RProcess server;
	_LIT(KScheduleServerExe, "Schexe");
	
	RDebug::Print(_L("Launching Task Scheduler via System Startup\n"));
	
	User::LeaveIfError(server.Create(KScheduleServerExe, _L("sysstartschexe")));
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	}
		

GLDEF_C TInt E32Main()
	{
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup=CTrapCleanup::New();	//can fail
	if (cleanup)
		{
		err = KErrNone;
		TRAP(err, LaunchTaskSchedulerL())
		delete cleanup;
		}
	return err;
	}
