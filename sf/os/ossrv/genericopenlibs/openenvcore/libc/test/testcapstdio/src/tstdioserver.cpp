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
// Name        : tstdioserver.cpp
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
#include "tstdioserver.h"
#include "tstdio.h"


//_LIT(KServerName, "tstdiocap");

CStdioTestServer* CStdioTestServer::NewL()
	{
	CStdioTestServer *server = new(ELeave) CStdioTestServer();
	CleanupStack::PushL(server);
	//server->ConstructL(KServerName);
RProcess	handle = RProcess();
	TParsePtrC	serverName(handle.FileName());
	server->StartL(serverName.Name());


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
	CStdioTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CStdioTestServer::NewL());
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

CTestStep* CStdioTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps

	//steps for captest.
	
	 if(aStepName == Krenamesys)
		{
		testStep = new CTestStdio(aStepName);
		}
		
	if(aStepName == Kfopensys1)
		{
		testStep = new CTestStdio(aStepName);
		}
		
	if(aStepName == Kfopensys2)
		{
		testStep = new CTestStdio(aStepName);
		}
	if(aStepName == Kfopenprivate1)
		{
		testStep = new CTestStdio(aStepName);
		}
	if(aStepName == Kfopenprivate2)
		{
		testStep = new CTestStdio(aStepName);
		}
		
	if(aStepName == Krenamesys)
		{
		testStep = new CTestStdio(aStepName);
		}
		
	if(aStepName == Krenameprivate)
		{
		testStep = new CTestStdio(aStepName);
		}
	
	if(aStepName == Kremovesys)
		{
		testStep = new CTestStdio(aStepName);
		}
	if(aStepName == Kremoveprivate)
		{
		testStep = new CTestStdio(aStepName);
		}
		
	return testStep;
	}

