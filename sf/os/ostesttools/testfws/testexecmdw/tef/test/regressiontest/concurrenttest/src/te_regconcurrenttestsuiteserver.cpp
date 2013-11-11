/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* Example file/test code to demonstrate how to develop a TestExecute Server
* Developers should take this project as a template and substitute their own
* for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
* in the process of the client. The client initialises the server by calling the
* one and only ordinal.
*
*/



/**
 @file te_concurrenttestsuiteserver.cpp
 @internalTechnology
*/

#include "te_regconcurrenttestsuiteserver.h"
#include "te_regwaitstep.h"
#include "te_regloopstep.h"
#include "te_regaccessstaticvarstep.h"
#include "te_regdummystep.h"
#include "te_regchecktestresultstep.h"

#ifdef TEST_SUITE1
_LIT(KServerName,"Te_RegConcurrentTestSuite1");
#elif TEST_SUITE2
_LIT(KServerName,"Te_RegConcurrentTestSuite2");
#elif TEST_SUITE3
_LIT(KServerName,"Te_RegConcurrentTestSuite3");
#endif
CTe_RegConcurrentTestSuiteServer* CTe_RegConcurrentTestSuiteServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_RegConcurrentTestSuiteServer * server = new (ELeave) CTe_RegConcurrentTestSuiteServer();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
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
	CTe_RegConcurrentTestSuiteServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_RegConcurrentTestSuiteServer::NewL());
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


CTestStep* CTe_RegConcurrentTestSuiteServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
    if(aStepName == KTe_RegWaitStep)
    	{
    	testStep = new CTe_RegWaitStep();
    	}
    else if(aStepName == KTe_RegLoopStep)
    	{
    	testStep = new CTe_RegLoopStep();
    	}
    else if(aStepName == KTe_RegAccessStaticVarStep)
    	{
    	testStep = new CTe_RegAccessStaticVarStep();
    	}
    else if(aStepName == KTe_RegDummyStep)
    	{
    	testStep = new CTe_RegDummyStep();
    	}
    else if (aStepName == KTe_RegCheckTestResultStep())
    	{
    	testStep = new CTe_RegCheckTestResultStep();
    	}

	return testStep;
	}
