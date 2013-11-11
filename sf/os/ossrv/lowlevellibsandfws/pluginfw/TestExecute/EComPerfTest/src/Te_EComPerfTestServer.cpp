// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Te_EComPerfLoggerServer.cpp
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @internalTechnology 
*/
#include "Te_EComPerfTestServer.h"
#include "Te_EComPerfLoggerStep.h" 
#include "Te_EComStartupStatePerfTestStep.h" 
#include "Te_EComClientRequestPerfTestStep.h" 
#include "Te_EComAccumulatedClientRequestPerfTestStep.h" 
#include "Te_EComPluginReDiscoverPerfTestStep.h"

_LIT(KServerName, "Te_EComPerfTest");

CEComPerfTestServer* CEComPerfTestServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CEComPerfTestServer * server = new (ELeave) CEComPerfTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
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
	CEComPerfTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CEComPerfTestServer::NewL());
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
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	err = err; // removes armv5 warning
	delete cleanup;
	return KErrNone;
    }

CTestStep* CEComPerfTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if (aStepName == KEComStartupStatePerfTest)
		{
		testStep = new CEComStartupStatePerfTest();
		}
	else if (aStepName == KEComPerfLogger)
		{
		testStep = new CEComPerfLogger();
		}
	else if (aStepName == KEComAccumulatedClientRequestsPerfTest)
		{
		testStep = new CEComAccumulatedClientRequestsPerfTest();
		}
	else if (aStepName == KEComClientRequestsPerfTest)
		{
		testStep = new CEComClientRequestsPerfTest();
		}
	else if	(aStepName == KEComPluginReDiscoverPerfTest)
		{
		testStep = new CEComPluginReDiscoverPerfTest();
		}
	return testStep;
	}

