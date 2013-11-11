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
* This contains CTestPlatSecServer. This class creates and initializes
* the test steps and return the pointer to TEF
*
*/


#include "testplatsecserver.h"
#include "testplatsecsetcapabilities.h"
#include "testplatseclaunchapp.h"
#include "testplatseccleanup.h"



/**
 Secure variant
 Much simpler, uses the new Rendezvous() call to sync with the client
*/
LOCAL_C void MainL()
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler*	sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTestPlatSecServer*	server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestPlatSecServer::NewL());
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
 Secure variant only
 Process entry point. Called by client using RProcess API
 @return - Standard Epoc error code on process exit
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
 Two Phase constructor
 @return CTestPlatSecServer object pointer
 @leave KErrNoMemory
 @leave system wide error codes
*/
CTestPlatSecServer* CTestPlatSecServer::NewL()
	{
	CTestPlatSecServer*	server=new(ELeave) CTestPlatSecServer();
	CleanupStack::PushL(server);
	RProcess handle = RProcess();
	TParsePtrC serverName(handle.FileName());
	server->ConstructL(serverName.Name());
	CleanupStack::Pop();
	return server;
	}

/**
 Constructor
*/
CTestPlatSecServer::CTestPlatSecServer()
	{
	}

/**
 Destructor
*/
CTestPlatSecServer::~CTestPlatSecServer()
	{
	}

/**
 Implementation of CTestServer pure virtual. 
 Creates the test step.
 @param aStepName - test step name to run.
 @return - A CTestStep derived instance
*/

CTestStep* CTestPlatSecServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep*	testStep = NULL;
	if(aStepName == _L("SetCapabilities"))
		{
		testStep = new CTestPlatSecSetCapabilities();
		}
    else if (aStepName == _L("LaunchApp"))
        {
        testStep = new CTestPlatSecLaunchApp();
        }
	else if (aStepName == _L("Cleanup"))
		{
		testStep = new CTestPlatSecCleanup();
		}
	return testStep;
	}

