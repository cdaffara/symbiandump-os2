/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * @file
 *
 * SWIS test server implementation
 */

#include "ttesttoolserver.h"
#include "ttesttoolstep.h"

_LIT(KServerName, "ttesttools");

/**
 * Called inside the MainL() function to create and start the CTestServer 
 * derived server.
 * @return Instance of the test server
 */
CTestToolServer* CTestToolServer::NewL()
	{
	CTestToolServer *server = new(ELeave) CTestToolServer();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTestToolServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CTestToolServer::NewL());
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
 * Server entry point
 * @return Standard Epoc error code on exit
 */
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
 * Implementation of CTestServer pure virtual
 * @return A CTestStep derived instance
 */
CTestStep* CTestToolServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if (aStepName == KTestToolListCertStep)	
		testStep = new CTestToolListCertStep();
	else if (aStepName == KTestToolGetTrustAppsStep)	
		testStep = new CTestToolGetTrustAppsStep();
	else if (aStepName == KTestToolGetTrustStep)	
		testStep = new CTestToolGetTrustStep();
	else if (aStepName == KTestToolListKeyStep)	
		testStep = new CTestToolListKeyStep();
	else if (aStepName == KTestToolGetPolicyStep)	
		testStep = new CTestToolGetPolicyStep();
	else if (aStepName == KTestToolParseFileStep)	
		testStep = new CTestToolParseFileStep();
	else if (aStepName == KTestToolCheckFileStep)
		testStep = new CTestToolCheckFileStep();

	return testStep;

	}
