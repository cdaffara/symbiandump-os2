/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file 
*/
#include "captestframeworkserver.h"
#include "captestframeworkstep.h"

_LIT(KServerName,"captestframework");

CCapTestFrameworkServer* CCapTestFrameworkServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CCapTestFrameworkServer * server = new (ELeave) CCapTestFrameworkServer();
	CleanupStack::PushL(server);
	
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CCapTestFrameworkServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CCapTestFrameworkServer::NewL());
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
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	
	TRAPD(err,MainL());
	delete cleanup;
	return err;
	}

CTestStep* CCapTestFrameworkServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	if (aStepName == KRunBasicCapabilityChecks)
		{
		return new CCapTestFrameworkStep(CCapTestFrameworkStep::EBasicChecks);
		}
	else if (aStepName == KRunThoroughCapabilityChecks)
		{
		return new CCapTestFrameworkStep(CCapTestFrameworkStep::EThoroughChecks);
		}

	return NULL;
	}

