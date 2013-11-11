/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* code at the __EDIT_ME__ tags
* for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
* in the process of the client. The client initialises the server by calling the
* one and only ordinal.
*
*/

/**
 @file 
*/
#include "T_UsbManagerServer.h"
#include "CCancelStartInterest.h"
#include "CCancelStopInterest.h"
#include "CStartPersonality.h"
#include "CStopPersonality.h"
#include "CStartNewPersonality.h"
#include "CSimCablePulling.h"
#include "CStartStopPersonality.h"
#include "CUsbComponentTest.h"
#include "CUsbMsComponentTest.h"


_LIT(KServerName,"T_UsbManagerServer");

CUsbManagerServer* CUsbManagerServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{

	CUsbManagerServer * server = new (ELeave) CUsbManagerServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.

	server->StartL(KServerName); 
	CleanupStack::Pop(server);
	return server;
	}

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CUsbManagerServer* server = NULL;
	// Create the CTestServer derived server

	TRAPD(err,server = CUsbManagerServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// Only a DLL on emulator for typhoon and earlier

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
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier


CTestStep* CUsbManagerServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// __EDIT_ME__ - Create your own test steps here
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created


	if(aStepName == KCancelStartInterest)
		testStep = new CCancelStartInterest();
	if(aStepName == KCancelStopInterest)
		testStep = new CCancelStopInterest();
	if(aStepName == KStartPersonality)
		testStep = new CStartPersonality();
	if(aStepName == KStopPersonality)
		testStep = new CStopPersonality();
	if(aStepName == KStartNewPersonality)
		testStep = new CStartNewPersonality();
	if(aStepName == KSimCablePulling)
		testStep = new CSimCablePulling();
	if(aStepName == KStartStopPersonality1)
		testStep = new CStartStopPersonality1();
	if(aStepName == KStartStopPersonality2)
		testStep = new CStartStopPersonality2();
	
	if(aStepName == KUsbLoadPersonalityNormal)
		testStep = new CUsbLoadPersonalityNormal();
	
	if(aStepName == KUsbLoadPersonalityAbNormal)
		testStep = new CUsbLoadPersonalityAbNormal();
	
	if(aStepName == KUsbLoadPersonalityMissing)
		testStep = new CUsbLoadPersonalityMissing();
	
	if(aStepName == KUsbTestOOM)
		testStep = new CUsbTestOOM();
	
	if(aStepName == KUsbSwitchPersonalityNormal)
		testStep = new CUsbSwitchPersonalityNormal();
	
	if(aStepName == KUsbSwitchPersonalityAbNormal)
		testStep = new CUsbSwitchPersonalityAbNormal();
	
	if(aStepName == KUsbStartStopPersonality1)
		testStep = new CUsbStartStopPersonality1();
	
	if(aStepName == KUsbStartStopPersonality2)
		testStep = new CUsbStartStopPersonality2();
	
	if(aStepName == KUsbStartStopPersonality3)
		testStep = new CUsbStartStopPersonality3();
	
	if(aStepName == KMemAllocationFailure)
		testStep = new CMemAllocationFailure();
	
	if(aStepName == KUsbMsccGetPersonality)
		testStep = new CUsbMsccGetPersonality();
	
	if(aStepName == KUsbMsccStartStop)
		testStep = new CUsbMsccStartStop();
		
	return testStep;
	}

