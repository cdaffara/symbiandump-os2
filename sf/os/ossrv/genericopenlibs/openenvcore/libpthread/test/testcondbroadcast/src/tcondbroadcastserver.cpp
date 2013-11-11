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
// Name        : tcondbroadcastserver.cpp
// Part of     : testcondbroadcast
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
#include "tcondbroadcastserver.h"
#include "tcondbroadcast.h"


_LIT(KServerName, "tcondbroadcast");

CCondbroadcastTestServer* CCondbroadcastTestServer::NewL()
	{
	CCondbroadcastTestServer *server = new(ELeave) CCondbroadcastTestServer();
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
	CCondbroadcastTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CCondbroadcastTestServer::NewL());
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

CTestStep* CCondbroadcastTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == KTestCond452)
		{
		testStep = new CTestCondbroadcast(aStepName);
		}
	else if(aStepName == KTestCond453)
		{
		testStep = new CTestCondbroadcast(aStepName);
		}
	else if(aStepName == KTestCond454)
		{
		testStep = new CTestCondbroadcast(aStepName);
		}
	else if(aStepName == KTestCond455)
		{
		testStep = new CTestCondbroadcast(aStepName);
		}
	else if(aStepName == KTestCond456)
		{
		testStep = new CTestCondbroadcast(aStepName);
		}
	else if(aStepName == KTestCond457)
		{
		testStep = new CTestCondbroadcast(aStepName);
		}
	else if(aStepName == KTestCond458)
		{
		testStep = new CTestCondbroadcast(aStepName);
		}
	else if(aStepName == KTestCond675)
		{
		testStep = new CTestCondbroadcast(aStepName);
		}
	else if(aStepName == KTestCond676)
		{
		testStep = new CTestCondbroadcast(aStepName);
		}
	else if(aStepName == KTestCond677)
		{
		testStep = new CTestCondbroadcast(aStepName);
		}
	
	return testStep;
	}

