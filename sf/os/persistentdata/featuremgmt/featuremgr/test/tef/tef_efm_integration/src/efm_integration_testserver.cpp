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
 @test
 @internalComponent - Internal Symbian test code 
*/
#include "efm_integration_testserver.h"

#include "efm_integration_teststeps.h"

 
_LIT(KServerName,"efm_integration_testserver");

CIntegrationEFMServer* CIntegrationEFMServer::NewL()
	{
	CIntegrationEFMServer * server = new (ELeave) CIntegrationEFMServer();
	CleanupStack::PushL(server);
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CIntegrationEFMServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CIntegrationEFMServer::NewL());
	if(!err)
		{
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

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

CTestStep* CIntegrationEFMServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
           
	if (aStepName == KSingleRomSectionTest)
		{
		testStep = new CSingleRomSectionTestStep();
		}
    else if (aStepName == KMultipleRomSectionsTest)
		{
		testStep = new CMultipleRomSectionsTestStep();
		}
    else if (aStepName == KFeatRegVerificationTest)
    	{
    	testStep = new CFeatRegVerificationTestStep();
    	}
	
	return testStep;
	}

