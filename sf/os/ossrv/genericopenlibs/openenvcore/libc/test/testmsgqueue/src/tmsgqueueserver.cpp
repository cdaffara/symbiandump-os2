/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



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
#include "tmsgqueueserver.h"
#include "tmsgqueue.h"


_LIT(KServerName, "tmsgqueue");
CMsgqueueTestServer::CMsgqueueTestServer()
	{
	}

CMsgqueueTestServer::~CMsgqueueTestServer()
	{
	}
CMsgqueueTestServer* CMsgqueueTestServer::NewL()
	{
	CMsgqueueTestServer *server = new(ELeave) CMsgqueueTestServer();
	CleanupStack::PushL(server);
	TRAPD(err,server->ConstructL(KServerName));
	if(err != KErrNone)
		{
		CleanupStack::PopAndDestroy(server);	
		server = NULL;
		User::Leave(err);
		}
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
	CMsgqueueTestServer* server = NULL;
	// Create the CTestServer derived server
	__UHEAP_MARK;
	TRAPD(err, server = CMsgqueueTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	server  = NULL;
	__UHEAP_MARKEND;
	delete sched;
	sched = NULL;
	}

/**
 * Server entry point
 * @return Standard Epoc error code on exit
 */
TInt main()
	{
	//__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL) 
		{
		return KErrNoMemory;  
		}
	TRAPD(err,MainL());
	if(err!=KErrNone)
		{
		}
	delete cleanup;
	//__UHEAP_MARKEND;
	
	return KErrNone;
	}

CTestStep* CMsgqueueTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	
	if(aStepName == KMessageQueueCreate)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KMessageQueueControl)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	
	if(aStepName == KIntgTest1)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest2)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest3)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest4)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest5)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest6)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest7)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest8)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest9)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest10)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest11)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest12)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest13)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest14)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest15)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest16)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest17)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KIntgTest6_1)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	
	if(aStepName == Kmsggettest1)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsggettest2)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsggettest3)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsggettest4)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsggettest5)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsggettest6)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == KMsggetCreateKey)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgctltest1)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgctltest2)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgctltest3)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgctltest4)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgctltest5)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgsndtest1)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgsndtest2)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgsndtest3)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgsndtest4)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgsndtest5)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgsndtest6)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgrcvtest1)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgrcvtest2)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgrcvtest3)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kmsgrcvtest4)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Ksndrcvtest1)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Ksndrcvtest2)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Ksndrcvtest3)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Ksndrcvtest4)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	if(aStepName == Kthsndrcvtest1)
		{
		testStep = new CTestMsgqueue(aStepName);
		}
	
	return testStep;
	}

