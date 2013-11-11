// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sessmgr.h"   // CSessionManager
#include "srvparams.h" // KServerName
#include "srvres.h"	   // TServerResources
#include "srvrepos_noc.h" // CServerRepository
#include <bacline.h>   // CCommandLineArguments
#include "backup.h"	   // CRepositoryBackupClient
#include "install.h"   // SWI watcher

enum
	{
	ESoftReset = 0x01,
	} TStartupOptions;

_LIT(KSoftReset, "--SoftReset");

static void ParseCmdLineOptionsL(TInt& aOptions)
	{
	CCommandLineArguments* args = CCommandLineArguments::NewLC();

	for (TInt i = 1; i < args->Count(); ++i)
		{
		if (args->Arg(i).Compare(KSoftReset) == 0)
			{
			aOptions |= ESoftReset;
			}
		}

	CleanupStack::PopAndDestroy(args);
	}

static void CloseTServerResources(TAny*)
	{
	// Ready to exit.
	TServerResources::Close();
	}

//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
static void RunServerL()
	{
	TInt options = 0;

#ifndef SYSLIBS_TEST
    // Set the server as a system critical thread.
	User::LeaveIfError(User::SetCritical(User::ESystemCritical));
#endif

	ParseCmdLineOptionsL(options);

	// NOTE: Insert TraceHeap install here,
	//		 when RAllocator is available.
	//
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(User::RenameThread(KServerName));

	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);

	CleanupStack::PushL(TCleanupItem(CloseTServerResources, 0));
	TServerResources::InitialiseL();

	CSessionManager::NewLC();

	CCentRepSWIWatcher* swiWatcher = 0;

		
	if( TServerResources::iInstallDirectory)
		{
		swiWatcher = CCentRepSWIWatcher::NewL(TServerResources::iFs);	
		CleanupStack::PushL(swiWatcher) ;
		swiWatcher->Start();
		}

	CRepositoryBackupClient* backupClient =
			CRepositoryBackupClient::NewL(TServerResources::iFs);

	CleanupStack::PushL(backupClient) ;

	backupClient->StartL();
#ifdef SYMBIAN_BAFL_SYSUTIL
	PERF_TEST_SERVER_START();

	TRAPD(err, CServerRepository::CheckROMReflashL());
	if(err != KErrNone)
		{
		if(err == KErrNoMemory)
			{
			User::LeaveNoMemory();
			}
		else
			{//Dont stop the centrep from starting up from any other error.
			__CENTREP_TRACE1("CENTREP: CServerRepository::CheckROMReflashL - Error = %d", err);
			}
		}
	PERF_TEST_SERVER_END();	
#endif

	// check command line options
	if (options & ESoftReset)
		{
		CServerRepository::RFSAllRepositoriesL();
		}

	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);

	// Ready to run
	CActiveScheduler::Start();

	// Delete backup client if it exists
	if(backupClient)
		CleanupStack::PopAndDestroy(backupClient);

	// Delete file watcher if it exists
	if(swiWatcher)
		CleanupStack::PopAndDestroy(swiWatcher);

	// Ready to exit.

	TServerResources::Close();

	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);//CSessionManager, s
	}

// Entry point for the server
TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt r = KErrNoMemory;
	if(cleanup)
		{
		TRAP(r, RunServerL());
		delete cleanup;
		}

	__UHEAP_MARKEND;

	return r;
	}

