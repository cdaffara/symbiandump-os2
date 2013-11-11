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
// Name        : tcondsignalserver.cpp
// Part of     : testcondsignal
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
#include "tcondsignalserver.h"
#include "tcondsignal.h"


_LIT(KServerName, "tcondsignal");

CCondsignalTestServer* CCondsignalTestServer::NewL()
	{
	CCondsignalTestServer *server = new(ELeave) CCondsignalTestServer();
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
	CCondsignalTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CCondsignalTestServer::NewL());
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

CTestStep* CCondsignalTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == KTestCond439)
		{
		testStep = new CTestCondsignal(aStepName);
		}
	if(aStepName == KTestCond440)
		{
		testStep = new CTestCondsignal(aStepName);
		}
	if(aStepName == KTestCond441)
		{
		testStep = new CTestCondsignal(aStepName);
		}
	if(aStepName == KTestCond442)
		{
		testStep = new CTestCondsignal(aStepName);
		}
	if(aStepName == KTestCond443)
		{
		testStep = new CTestCondsignal(aStepName);
		}
	if(aStepName == KTestCond444)
		{
		testStep = new CTestCondsignal(aStepName);
		}
	if(aStepName == KTestCond445)
		{
		testStep = new CTestCondsignal(aStepName);
		}
	if(aStepName == KTestCond678)
		{
		testStep = new CTestCondsignal(aStepName);
		}
	if(aStepName == KTestCond679)
		{
		testStep = new CTestCondsignal(aStepName);
		}
	if(aStepName == KTestCond680)
		{
		testStep = new CTestCondsignal(aStepName);
		}
	
	return testStep;
	}

