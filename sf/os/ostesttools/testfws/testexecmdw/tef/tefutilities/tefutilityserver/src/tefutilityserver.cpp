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
* Basic Test Server implementation to instantiate tests and return a pointer to run tests
*
*/



/**
 @file TEFUtilityServer.cpp
*/

#include "tefutiltityserver.h"
#include "tefbuildinfologstep.h"
#include "tefformatdrive.h"

/**
 * @name Constant Literals used
 */
_LIT(KServerName,"TEFUtilityServer");

/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
CTEFUtilityServer* CTEFUtilityServer::NewL()
	{
	CTEFUtilityServer*	server = new (ELeave) CTEFUtilityServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
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
	CTEFUtilityServer*	server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTEFUtilityServer::NewL());
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
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
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
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
CTestStep* CTEFUtilityServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KPrintBuildInfo)
		{
#if !(defined TEF_LITE)
		testStep = new CTEFBuildInfoLogStep();
#endif
		}
	
	if(aStepName == KFormatDrive)
		{
		testStep = new CTEFFormatDrive();
		}
			 
	return testStep;
	}
