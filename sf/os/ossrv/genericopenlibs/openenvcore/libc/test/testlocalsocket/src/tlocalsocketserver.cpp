// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tlocalsocketserver.cpp
// 
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
#include "tlocalsocketserver.h"
#include "tlocalsocket.h"


_LIT(KServerName, "tlocalsocket");

CLocalSocketTestServer* CLocalSocketTestServer::NewL()
	{
	CLocalSocketTestServer *server = new(ELeave) CLocalSocketTestServer();
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
	CLocalSocketTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CLocalSocketTestServer::NewL());
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

CTestStep* CLocalSocketTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == KTestBind)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestLseek)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestFstat)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestThreadSocketRead)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestThreadSocketWrite)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestThreadSocketRecv)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestThreadSocketSend)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestMultProc)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestMultThread)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestSetGetSockOpt)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestSetSockOptNegative1)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestSetSockOptNegative2)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestSetSockOptNegative3)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestSetSockOptNegative4)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestGetSockOptNegative1)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestGetSockOptNegative2)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestGetSockOptNegative3)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestGetSockOptNegative4)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestLocalSockIoctl)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestLocalSockFcntl)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestNegativeBind1)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestNegativeBind2)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestNegativeBind3)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestNegativeBind4)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestNegativeBind5)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestListenonUnixsoc)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestNonblockAcceptSetfl)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestNonblockAcceptSelect)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestNonblockingRead)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestNonblockingRecv)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestGetSockName)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestGetPeerName)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestThreadSendto)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestThreadRecvfrom)
		{
		testStep = new CTestLocalSocket(aStepName);
		}
	if(aStepName == KTestLocalSockMultipleBind)
		{
		testStep = new CTestLocalSocket(aStepName);
		}	
	return testStep;
	}
