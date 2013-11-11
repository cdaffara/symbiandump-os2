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
// Name        : twideapisserver.cpp
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
#include "twideapisserver.h"
#include "twideapis.h"


_LIT(KServerName, "twideapis");

CWideApiTestServer* CWideApiTestServer::NewL()
	{
	CWideApiTestServer *server = new(ELeave) CWideApiTestServer();
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
	CWideApiTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CWideApiTestServer::NewL());
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


CTestStep* CWideApiTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == Kwfreopen_val)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwfreopen_valm)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwfreopen_valinv)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwfdopen_val)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwfdopen_ivalm)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kgetws_null)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kgetws_val)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kputws_null)
		{
		testStep = new CTestWideApi(aStepName);
		}		
	else if(aStepName == Kputws_val1)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kputws_val2)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwremove_val1)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwremove_val2)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwremove_val3)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwremove_null)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwfdopen_ivalm1)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwpopen_1)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwfreopen1)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwfreopen2)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwfreopen3)
		{
		testStep = new CTestWideApi(aStepName);
		}
	else if(aStepName == Kwfreopen4)
		{
		testStep = new CTestWideApi(aStepName);
		}
	return testStep;
	}

