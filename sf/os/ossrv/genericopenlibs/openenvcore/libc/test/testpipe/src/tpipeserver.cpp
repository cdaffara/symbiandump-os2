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
// Name        : TPipeServer.cpp
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
#include "tpipeserver.h"
#include "testpipe.h"

_LIT(KServerName, "tpipe");

CPipeTestServer* CPipeTestServer::NewL()
	{
	CPipeTestServer *server = new(ELeave) CPipeTestServer();
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
	CPipeTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CPipeTestServer::NewL());
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
	TRAP_IGNORE(MainL());
	return KErrNone;
	}

CTestStep* CPipeTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if (aStepName == KTestPipe)
		testStep = new CTestPipe(aStepName);
	else if (aStepName == KPipeCheckWriteOnReadfd)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KPipeCheckReadOnWritefd)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KPipeWriteRead)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KPipeCheckWriteFd)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KPipeCheckReadFd)
		testStep = new CTestPipe(aStepName);
	
	/*popen tests*/	
	else if(aStepName==KPopenPipeCommandRead)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KMultiplePopenPipeCommandRead)
		testStep = new CTestPipe(aStepName);	
	else if(aStepName==KPopenPipeOEExeRead)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KPopenPipeInvalidPathRead)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KPopenPipeInvalidCommand)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KPopenPipeInvalidMode)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KPopenPipeWrite)
		testStep = new CTestPipe(aStepName);	
	else if(aStepName==KPopenBlockingRead)
		testStep = new CTestPipe(aStepName);	
	/*popen3 tests*/
	else if(aStepName==KPopen3PipeCommandRead)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KMultiplePopen3PipeCommandRead)
		testStep = new CTestPipe(aStepName);	
	else if(aStepName==KPopen3PipeOEExeRead)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KPopen3PipeInvalidPathRead)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KPopen3PipeInvalidCommand)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KTestProcessPopen3ChitChat)
		testStep = new CTestPipe(aStepName);	
	else if(aStepName==KPopen3ReadWriteTest)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KTestSystem)
		testStep = new CTestPipe(aStepName);
	/*Negative tests*/
	else if(aStepName==KLseekpipetest)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KOpenMaxfdPipetest)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KSimultaneousOpenfdPipetest)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KFopenMaxPopenTest)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KFopenMaxPopen3Test)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KTestEnvPopen3)
		testStep = new CTestPipe(aStepName);
	
	else if(aStepName==KTestsystem_LongName)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KTestwsystem_LongName)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KTestpopen_LongName)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KTestwpopen_LongName)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KTestpopen3_LongName)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KTestwpopen3_LongName)
		testStep = new CTestPipe(aStepName);
	else if(aStepName==KTestPipeWaitForData)
	    testStep = new CTestPipe(aStepName);
	
	return testStep;
	}
