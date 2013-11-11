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
// Name        : tselectserver.cpp
//

#include <c32comm.h>

#if defined (__WINS__)
#define PDD_NAME		_L("ECDRV")
#else
#define PDD_NAME		_L("EUART1")
#define PDD2_NAME		_L("EUART2")
#define PDD3_NAME		_L("EUART3")
#define PDD4_NAME		_L("EUART4")
#endif

#define LDD_NAME		_L("ECOMM")

/**
 * @file
 *
 * Pipe test server implementation
 */
#include "taeselectserver.h"
#include "taeselect.h"


_LIT(KServerName, "taeselect");

CAESelectTestServer* CAESelectTestServer::NewL()
	{
	CAESelectTestServer *server = new(ELeave) CAESelectTestServer();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

static void InitCommsL()
    {
    TInt ret = User::LoadPhysicalDevice(PDD_NAME);
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);

#ifndef __WINS__
    ret = User::LoadPhysicalDevice(PDD2_NAME);
    ret = User::LoadPhysicalDevice(PDD3_NAME);
    ret = User::LoadPhysicalDevice(PDD4_NAME);
#endif

    ret = User::LoadLogicalDevice(LDD_NAME);
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);
    ret = StartC32();
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);
    }

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	//InitCommsL();
	
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CAESelectTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CAESelectTestServer::NewL());
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
TInt main()
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

CTestStep* CAESelectTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == KTestTimeout )
		{
		testStep = new CTestAESelect(aStepName);
		}
	else if(aStepName == KTestASelect )
		{
		testStep = new CTestAESelect(aStepName);
		}
	else if(aStepName == KTestCancelASelect)
		{
		testStep = new CTestAESelect(aStepName);
		}
	else if(aStepName == KTestAselectRequestStatus)
		{
		testStep = new CTestAESelect(aStepName);
		}
	else if(aStepName == KTestMultipleASelect)
		{
		testStep = new CTestAESelect(aStepName);
		}
	else if(aStepName == KTestSelectwithEselect )
		{
		testStep = new CTestAESelect(aStepName);		
		}
	else if(aStepName == KTestESelect )
		{
		testStep = new CTestAESelect(aStepName);
		}
	else if(aStepName == KTestEselectRequestStatus )
		{
		testStep = new CTestAESelect(aStepName);
		}
	else if(aStepName == KTestESelectTimeout )
		{
		testStep = new CTestAESelect(aStepName);
		}
	else if(aStepName == KTestEselectInvalidInput_01 )
		{
		testStep = new CTestAESelect(aStepName);
		}
	else if(aStepName == KTestEselectInvalidInput_02 )
		{
		testStep = new CTestAESelect(aStepName);		
		}
	return testStep;
	}

