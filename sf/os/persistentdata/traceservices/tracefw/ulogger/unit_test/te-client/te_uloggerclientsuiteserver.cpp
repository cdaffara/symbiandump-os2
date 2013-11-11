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
 @file Te_uloggerclientSuiteServer.cpp
 @internalTechnology
*/

#include "te_uloggerclientsuiteserver.h"
#include "testconnectstep.h"
#include "teststartstep.h"
#include "teststopstep.h"
#include "testsetprimaryfltstep.h"
#include "testgetprimaryfilterslstep.h"
#include "testgetsecondaryfilterslstep.h"
#include "testgettracebuffersizestep.h"
#include "testremoveprimaryfltstep.h"
#include "testresizetracebuffersizestep.h"
#include "testsetdatanotificationsizestep.h"
#include "testsetsecondaryfilterallstep.h"
#include "testsetsecondaryfltstep.h"
#include "testversionstep.h"
#include "testdeactivateoutputpluginstep.h"
#include "testruloggerapi_ext.h"

_LIT(KServerName,"UloggerClientAPIServer");
CTestUloggerClientApi* CTestUloggerClientApi::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTestUloggerClientApi * server = new (ELeave) CTestUloggerClientApi();
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
	CTestUloggerClientApi* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestUloggerClientApi::NewL());
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


CTestStep* CTestUloggerClientApi::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	
              if(aStepName == KTestConnectStep)
                            testStep = new CTestConnectStep();
                            
              else if(aStepName == KTestStartStep)
                            testStep = new CTestStartStep();
                            
              else if(aStepName == KTestStopStep)
                            testStep = new CTestStopStep();
                            
              else if(aStepName == KTestSetPrimaryFltStep)
                            testStep = new CTestSetPrimaryFltStep();                            
                            
              else if(aStepName == KTestGetPrimaryFiltersLStep)
                            testStep = new CTestGetPrimaryFiltersLStep();
                            
              else if(aStepName == KTestGetSecondaryFiltersLStep)
                            testStep = new CTestGetSecondaryFiltersLStep();
                            
              else if(aStepName == KTestGetTraceBufferSizeStep)
                            testStep = new CTestGetTraceBufferSizeStep();
                            
              else if(aStepName == KTestRemovePrimaryFltStep)
                            testStep = new CTestRemovePrimaryFltStep();
                            
              else if(aStepName == KTestResizeTraceBufferSizeStep)
                            testStep = new CTestResizeTraceBufferSizeStep();
                            
              else if(aStepName == KTestSetDataNotificationSizeStep)
                            testStep = new CTestSetDataNotificationSizeStep();
                            
              else if(aStepName == KTestSetSecondaryFilterAllStep)
                            testStep = new CTestSetSecondaryFilterAllStep();
                            
              else if(aStepName == KTestSetSecondaryFltStep)
                            testStep = new CTestSetSecondaryFltStep();
                            
              else if(aStepName == KTestVersionStep)
                            testStep = new CTestVersionStep();
              
              else if(aStepName == KTestDeactivateOutputPluginStep)
                            testStep = new CTestDeactivateOutputPluginStep();
              
              else if(aStepName == KTestRULoggerAPIStep)
                  testStep = new CTestRULoggerAPIStep();
                            
	return testStep;
	}
