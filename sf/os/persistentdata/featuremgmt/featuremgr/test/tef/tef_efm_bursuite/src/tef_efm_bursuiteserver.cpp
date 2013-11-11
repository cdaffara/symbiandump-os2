// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent
*/

#include "tef_efm_bursuiteserver.h"
#include "burstep.h"
#include "notifystep.h"
#include "queuestep.h"
#include "statestep.h"

_LIT( KServerName, "tef_efm_bursuite" );

/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
CTefEfmBURSuite* CTefEfmBURSuite::NewL()
	{
	CTefEfmBURSuite* server = new (ELeave) CTefEfmBURSuite();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

/**
 * Uses the Rendezvous() call to sync with the client
 */
LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTefEfmBURSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTefEfmBURSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/**
 * @return - Standard Epoc error code on process exit
 * Process entry point. Called by client using RProcess API
 */
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
		
	delete cleanup;
	__UHEAP_MARKEND;
	
	return err;
	}


/**
 * This function creates the individual test steps required by the script.
 * They are created "just in time" when the worker thread is created. More 
 * test steps can be added if necessary.
 * @return CTestStep A derived instance of CTestStep
 */
CTestStep* CTefEfmBURSuite::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	     if( aStepName == KNotifyBackupStep )  { testStep = new CNotifyBackupStep();  }
	else if( aStepName == KNotifyRestoreStep ) { testStep = new CNotifyRestoreStep(); }
	else if( aStepName == KBURStep )           { testStep = new CBURStep();           }
	else if( aStepName == KQUEUEBackupStep )   { testStep = new CQueueBackupStep();   }
	else if( aStepName == KQUEUERestoreStep )  { testStep = new CQueueRestoreStep();  }
	else if( aStepName == KStateStep )         { testStep = new CStateStep();         }

	// default case
	else
		{
		ERR_PRINTF1( _L("CTefEfmBURSuite::CreateTestStep -- Unknown test step.") );
		}
              
	return testStep;
	}


