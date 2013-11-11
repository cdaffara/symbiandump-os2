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


#include "tupsintegserver.h"
#include "tupsclientstep.h"
#include <bacline.h>
#include "tupspropertystep.h"
#include "tups_utils.h"
#include "tups_minortests.h"
#include "tupsdbmanagementstep.h"
#include "tupscoordinatorstep.h"


CTUpsSuite* CTUpsSuite::NewL(const TDesC& aName)
/**
 * @return - Instance of the test server
 */
	{
	CTUpsSuite * server = new (ELeave) CTUpsSuite();
	CleanupStack::PushL(server);
	server->ConstructL(aName);
	CleanupStack::Pop(server);
	return server;
	}


LOCAL_C void MainL()
/**
 * Main implementation
 */
	{

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTUpsSuite* server = NULL;
	
	// gets name of executable, this will allow correct cloning with SETCAP
	CCommandLineArguments* args = CCommandLineArguments::NewLC();
	TPtrC exeName = args->Arg(0);
	TParse fullName;
	fullName.Set(exeName, NULL, NULL);   
	CleanupStack::PopAndDestroy(args);
	
	// Create the CTestServer derived server
	TRAPD(err,server = CTUpsSuite::NewL(fullName.Name()));
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


CTestStep* CTUpsSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName == KUPSClientStep)
		testStep = new CUpsClientStep();
	else if (aStepName == KPropertyStep)
		testStep = new CPropertyStep();
	else if (aStepName == KCloseUps)
		testStep = new CCloseUpsStep();
	else if (aStepName == KProtServAuthorisationStep)
		testStep = new CNonProtServAuthorisation();
	else if (aStepName == KUPSDbManagementStep)
		testStep = new CUPSDbManagementStep();
	else if (aStepName == KUPSCoordinatorStep)
		testStep = new CUPSCoordinatorStep();
	else if (aStepName == KDyingClientStep)
		testStep = new CDyingClient();
	else if (aStepName == KCancelPromptStep)
		testStep = new CCancelUpsPrompt();
	else if (aStepName == KSimpleRequestStep)
		testStep = new CSimpleUpsRequest();
	else if (aStepName == KMultithreadStep)
		testStep = new CMultiTread();
	
	return testStep;
	}
