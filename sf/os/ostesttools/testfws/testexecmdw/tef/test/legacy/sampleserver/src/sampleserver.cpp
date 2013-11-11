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
* code at the __EDIT_ME__ tags
* for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
* in the process of the client. The client initialises the server by calling the
* one and only ordinal.
*
*/



/**
 @file SampleServer.cpp
*/
#include "sampleserver.h"
#include "samplestep.h"

// __EDIT_ME__ - Substitute the name of your test server 
_LIT(KServerName,"SampleServer");
// __EDIT_ME__ - Use your own server class name
CSampleServer* CSampleServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	// __EDIT_ME__ new your server class here
	CSampleServer * server = new (ELeave) CSampleServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.

	//server->StartL(KServerName); 
	server-> ConstructL(KServerName);
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
	// __EDIT_ME__ Your server name
	CSampleServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CSampleServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// Only a DLL on emulator

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

// __EDIT_ME__ - Use your own server class name
CTestStep* CSampleServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// __EDIT_ME__ - Create your own test steps here
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created


	if(aStepName == KSampleStep1)
		testStep = new CSampleStep1();
	else if(aStepName == KSampleStep2)
		testStep = new CSampleStep2(*this);
	else if(aStepName == KSampleStep3)
		testStep = new CSampleStep3(*this);
	else if(aStepName == KSampleStep4)
		testStep = new CSampleStep4();
	else if(aStepName == KSampleStep5)
		testStep = new CSampleStep5();	
	else if(aStepName == KSampleStep6)
		testStep = new CSampleStep6();
	else if(aStepName == KSampleStep7)
		testStep = new CSampleStep7();
	else if(aStepName == KSampleStep8)
		testStep = new CSampleStep8();
	else if(aStepName == KSampleStep9)
		testStep = new CSampleStep9();
	else if(aStepName == KSampleStep10)
		testStep = new CSampleStep10();
	else if(aStepName == KSampleStep11)
		testStep = new CSampleStep11();
	else if(aStepName == KSampleStep12)
		testStep = new CSampleStep12();
	else if(aStepName == KSampleStep13)
		testStep = new CSampleStep13();
	return testStep;
	}

