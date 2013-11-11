// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include "Tef_SSndServer.h"
#include "ssndteststep.h"

/**
Called inside the MainL() function to create and start the
CTestServer derived server.
@return - Instance of the test server
*/
CSSndTestServer* CSSndTestServer::NewL()
	{
	CSSndTestServer * server = new (ELeave) CSSndTestServer();
	CleanupStack::PushL(server);

	// This test server uses different name after setcap
	TFileName exename = RProcess().FileName();
	TParsePtr pathparser(exename);

	// CServer base class call
	server->StartL(pathparser.Name());
	CleanupStack::Pop(server);
	return server;
	}
	
LOCAL_C void MainL()
	{
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
	
	CActiveScheduler* sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CSSndTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CSSndTestServer::NewL());
	if (KErrNone == err)
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

CTestStep* CSSndTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if (aStepName == KResetDb)
		{
		testStep = new CResetDbStep();
		}
	else if (aStepName == KGetSound)
		{
		testStep = new CGetSoundStep();
		}
	else if (aStepName == KSetSound)
		{
		testStep = new CSetSoundStep();
		}
	else if (aStepName == KCorruptSound)
		{
		testStep = new CCorruptSoundStep();
		}
	else if (aStepName == KConcurrentSetSound)
		{
		testStep = new CConcurrentStep();
		}
	else if (aStepName == KLockoutTest)
		{
		testStep = new CLockoutTestStep();
		}
	else if (aStepName == KSystemSoundFileTest)
		{
		testStep = new CSystemSoundFileTestStep();
		}
	else if (aStepName == KGapTest)
		{
		testStep = new CGapTestStep();
		}
	else if (aStepName == KSoundFileNoPathTest)
		{
		testStep = new CSoundFileNoPathTestStep();
		}

	return testStep;
	}
