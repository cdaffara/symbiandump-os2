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
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

#include "Te_floating_scheduleSuiteServer.h"
#include "persist_hometimeStep.h"
#include "persist_utcStep.h"
#include "persist_mixedStep.h"
#include "persist_hometime_floatStep.h"
#include "transient_hometimeStep.h"
#include "transient_utcStep.h"
#include "transient_mixedStep.h"
#include "transient_hometime_floatStep.h"
#include "hometime_floatStep.h"
#include "DEF061595_Step.h"

_LIT(KServerName,"Te_floating_scheduleSuite");
CTe_floating_scheduleSuite* CTe_floating_scheduleSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_floating_scheduleSuite * server = new (ELeave) CTe_floating_scheduleSuite();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	server->StartL(KServerName);
	//server->ConstructL(KServerName);
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
	CTe_floating_scheduleSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_floating_scheduleSuite::NewL());
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


CTestStep* CTe_floating_scheduleSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
              if(aStepName == Kpersist_hometimeStep)
                            testStep = new Cpersist_hometimeStep();
              else if(aStepName == Kpersist_utcStep)
                            testStep = new Cpersist_utcStep();
              else if(aStepName == Kpersist_mixedStep)
                            testStep = new Cpersist_mixedStep();
              else if(aStepName == Kpersist_hometime_floatStep)
                            testStep = new Cpersist_hometime_floatStep();
              else if(aStepName == Ktransient_hometimeStep)
                            testStep = new Ctransient_hometimeStep();
              else if(aStepName == Ktransient_utcStep)
                            testStep = new Ctransient_utcStep();
              else if(aStepName == Ktransient_mixedStep)
                            testStep = new Ctransient_mixedStep();
              else if(aStepName == Ktransient_hometime_floatStep)
                            testStep = new Ctransient_hometime_floatStep();
              else if(aStepName == Khometime_floatStep)
                            testStep = new Chometime_floatStep();
              else if(aStepName == KDEF061595_Step)
                            testStep = new CDEF061595_Step();
              
	return testStep;
	}
