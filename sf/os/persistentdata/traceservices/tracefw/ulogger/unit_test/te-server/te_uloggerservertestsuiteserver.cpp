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
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file Te_UloggerServerTestSuiteServer.cpp
 @internalTechnology
*/

#include "te_uloggerservertestsuiteserver.h"
#include "uloggerservertest0step.h"
#include "uloggerservertest1step.h"
#include "uloggerservertest2step.h"
#include "uloggerservertest3step.h"
#include "uloggerservertest4step.h"
#include "uloggerservertest5step.h"
#include "uloggerservertest6step.h"

using namespace Ulogger;
_LIT(KServerNameTest,"Te_UloggerServerTestSuite");
CTe_UloggerServerTestSuite* CTe_UloggerServerTestSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_UloggerServerTestSuite * server = new (ELeave) CTe_UloggerServerTestSuite();
	CleanupStack::PushL(server);

	server->ConstructL(KServerNameTest);
	CleanupStack::Pop(server);
	return server;
	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_UloggerServerTestSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_UloggerServerTestSuite::NewL());
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


CTestStep* CTe_UloggerServerTestSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	
              if(aStepName == KUloggerServerTest0Step)
                            testStep = new CUloggerServerTest0Step();
              else if(aStepName == KUloggerServerTest1Step)
                            testStep = new CUloggerServerTest1Step();
              else if(aStepName == KUloggerServerTest2Step)
                            testStep = new CUloggerServerTest2Step();
              else if(aStepName == KUloggerServerTest3Step)
            	  			testStep = new CUloggerServerTest3Step();
              else if(aStepName == KUloggerServerTest4Step)
            	  			testStep = new CUloggerServerTest4Step();
              else if(aStepName == KUloggerServerTest5Step)
            	  			testStep = new CUloggerServerTest5Step();
              else if(aStepName == KUloggerServerTest6Step)
            	  			testStep = new CUloggerServerTest6Step();
	return testStep;
	}
