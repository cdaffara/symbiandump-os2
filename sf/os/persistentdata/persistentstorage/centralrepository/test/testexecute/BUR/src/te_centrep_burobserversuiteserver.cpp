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
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file
*/

#include "te_centrep_burobserversuiteserver.h"
#include "restorenotifications.h"

_LIT(KServerName,"Te_centrep_BURObserverSuite");
CTe_centrep_BURObserverSuite* CTe_centrep_BURObserverSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_centrep_BURObserverSuite * server = new (ELeave) CTe_centrep_BURObserverSuite();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_centrep_BURObserverSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_centrep_BURObserverSuite::NewL());
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
 * Secure variant only
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


CTestStep* CTe_centrep_BURObserverSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the test step is execute. 
	// But all test steps will be run in the same thread because this
	// test server is loaded with the switch of "SharedData".
	// More test steps can be added below 
              if(aStepName == KRestoreNotificationsInitialiseStep)
                            testStep = new CRestoreNotificationsInitialiseStep();
              else if(aStepName == KRestoreNotificationsRequestStep)
                            testStep = new CRestoreNotificationsRequestStep();
              else if(aStepName == KRestoreNotificationsWaitStep)
                            testStep = new CRestoreNotificationsWaitStep();
              else if(aStepName == KRestoreNotificationsReleaseStep)
                            testStep = new CRestoreNotificationsReleaseStep();
                     
	return testStep;
	}
