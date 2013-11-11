/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
*/

#include "tupsdb.h" // TEF Test Server
#include "tupsdbstep.h" // TEF Steps Test Code


CTUpsDbServer* CTUpsDbServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{	
	CTUpsDbServer * server = new (ELeave) CTUpsDbServer();
	CleanupStack::PushL(server);

	server->ConstructL(KDbServerName);
	CleanupStack::Pop(server);
	return server;
	}
	

LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	
	CActiveScheduler *sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CTUpsDbServer* server = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,server = CTUpsDbServer::NewL());
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
	

CTestStep* CTUpsDbServer::CreateTestStep(const TDesC& aStepName)
	{
	// Initialise test step object to NULL if no TEF steps are assigned
	CTestStep* testStep = NULL;

	if(aStepName == KStepPerformance)
		testStep = new CUpsDbStepPerformance();
	else if (aStepName == KStepUnitTest)
		testStep = new CUpsDbStepUnitTest();

	return testStep;
	}
