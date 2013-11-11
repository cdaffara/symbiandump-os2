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
// Tests for log engine backup and restore
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file
 @internalComponent
*/

#include "backuprestoreserver.h"
#include "backuprestorestep.h"

_LIT(KServerName,"teflogengbur");
CLogEngTestExecuteServer* CLogEngTestExecuteServer::NewL()
/**
 @return - Instance of the test server
 Same code for Secure and non-secure variants
 Called inside the MainL() function to create and start the
 CTestServer derived server.
 */
	{
	CLogEngTestExecuteServer *server = new (ELeave) CLogEngTestExecuteServer();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CLogEngTestExecuteServer *server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CLogEngTestExecuteServer::NewL());
	if(err == KErrNone)
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
 @return - Standard Epoc error code on process exit
 Process entry point. Called by client using RProcess API
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


CTestStep* CLogEngTestExecuteServer::CreateTestStep(const TDesC& aStepName)
/**
 @return - A CTestStep derived instance
 Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
	if(aStepName == KBackupRestoreInitializeStep)
		testStep = new CBackupRestoreInitializeStep();
	if(aStepName == KStopLogServerStep)
		testStep = new CStopLogServerStep();
	if(aStepName == KBackupRestoreVerifyStep)
		testStep = new CBackupRestoreVerifyStep();

	return testStep;
	}

