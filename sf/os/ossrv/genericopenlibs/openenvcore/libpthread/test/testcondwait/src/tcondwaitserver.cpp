// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tcondwaitserver.cpp
// Part of     : testcondwait
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
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
#include "tcondwaitserver.h"
#include "tcondwait.h"


_LIT(KServerName, "tcondwait");

CCondwaitTestServer* CCondwaitTestServer::NewL()
	{
	CCondwaitTestServer *server = new(ELeave) CCondwaitTestServer();
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
	CCondwaitTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CCondwaitTestServer::NewL());
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

CTestStep* CCondwaitTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == KTestCond405)
		{
		testStep = new CTestCondwait(aStepName);
		}
	else if(aStepName == KTestCond406)
		{
		testStep = new CTestCondwait(aStepName);
		}
	else if(aStepName == KTestCond407)
		{
		testStep = new CTestCondwait(aStepName);
		}
	else if(aStepName == KTestCond408)
		{
		testStep = new CTestCondwait(aStepName);
		}
	else if(aStepName == KTestCond409)
		{
		testStep = new CTestCondwait(aStepName);
		}
	else if(aStepName == KTestCond410)
		{
		testStep = new CTestCondwait(aStepName);
		}
	else if(aStepName == KTestCond411)
		{
		testStep = new CTestCondwait(aStepName);
		}
	else if(aStepName == KTestCond413)
		{
		testStep = new CTestCondwait(aStepName);
		}
	else if(aStepName == KTestCond414)
		{
		testStep = new CTestCondwait(aStepName);
		}
	else if(aStepName == KTestCond415)
		{
		testStep = new CTestCondwait(aStepName);
		}
	else if(aStepName == KTestCond681)
		{
		testStep = new CTestCondwait(aStepName);
		}
	else if(aStepName == KTestCond682)
		{
		testStep = new CTestCondwait(aStepName);
		}
	else if(aStepName == KTestCondTWait1)
		{
		testStep = new CTestCondwait(aStepName);
		}
	return testStep;
	}

