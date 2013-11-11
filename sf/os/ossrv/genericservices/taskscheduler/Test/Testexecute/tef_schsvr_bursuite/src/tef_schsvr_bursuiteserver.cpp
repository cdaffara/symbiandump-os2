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

/**
 @file
*/

#include "tef_schsvr_bursuiteserver.h"
#include "comparestep.h"
#include "initialisestep.h"
#include "apistep.h"
#include "multistep.h"
#include "burstep.h"
#include "mergestep.h"

_LIT(KServerName,"tef_schsvr_bursuite");
CTefSchsvrBURSuite* CTefSchsvrBURSuite::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTefSchsvrBURSuite* server = new (ELeave) CTefSchsvrBURSuite();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
/**
 * Uses the Rendezvous() call to sync with the client
 */
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTefSchsvrBURSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTefSchsvrBURSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Process entry point. Called by client using RProcess API
 */
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


CTestStep* CTefSchsvrBURSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
              if(aStepName == KCompareStep)
                            testStep = new CCompareStep();
              else if(aStepName == KInitialiseStep)
                            testStep = new CInitialiseStep();
              else if(aStepName == KAPIStep)
                            testStep = new CAPIStep();
              else if(aStepName == KMultiStep)
                            testStep = new CMultiStep();
              else if(aStepName == KBURStep)
                            testStep = new CBURStep();
              else if(aStepName == KMergeStep)
                            testStep = new CMergeStep();              
              
	return testStep;
	}
