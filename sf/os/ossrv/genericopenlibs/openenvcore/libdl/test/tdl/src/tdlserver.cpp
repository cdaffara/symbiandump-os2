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
// Name        : tdlserver.cpp
// tdl test server implementation
// 
//

/**
 @file
*/
#include "tdlserver.h"
#include "tdl.h"


_LIT(KServerName, "tdl");

CTestDlByNameServer* CTestDlByNameServer::NewL()
	{
	CTestDlByNameServer *server = new(ELeave) CTestDlByNameServer();
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
	CTestDlByNameServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CTestDlByNameServer::NewL());
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
 * Server entry point
 * @return Standard Epoc error code on exit
 */
TInt E32Main()
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

CTestStep* CTestDlByNameServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if(aStepName == KDlSymLkupWithHandle)
		testStep = new CTestDlByName(aStepName);
	else if(aStepName == KDlSymLkupWithRTLD_DEFAULT)
		testStep = new CTestDlByName(aStepName);
	else if(aStepName == KDlLoadByVar)
		testStep = new CTestDlByName(aStepName);
	else if(aStepName == KDlLoadWithInvalidArg)
		testStep = new CTestDlByName(aStepName);
	else if(aStepName == KDlMutexOnOpenHandle)
		testStep = new CTestDlByName(aStepName);
	else if(aStepName == KOpenNonStdDllOrdinalLookup)
		testStep = new CTestDlByName(aStepName);
	else if(aStepName == KOpenNonStdDllSymbolicLookup)
		testStep = new CTestDlByName(aStepName);
	else if(aStepName == KOpenNonStdDllSymbolicLookupWithRTLD_DEFAULT)
		testStep = new CTestDlByName(aStepName);
	
	return testStep;
	}

