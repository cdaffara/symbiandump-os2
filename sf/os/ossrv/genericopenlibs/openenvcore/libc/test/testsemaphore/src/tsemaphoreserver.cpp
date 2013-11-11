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
#include "tsemaphoreserver.h"
#include "tsemaphore.h"


_LIT(KServerName, "tsemaphore");

CSemaphoreTestServer* CSemaphoreTestServer::NewL()
	{
	CSemaphoreTestServer *server = new(ELeave) CSemaphoreTestServer();
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
	CSemaphoreTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CSemaphoreTestServer::NewL());
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

CTestStep* CSemaphoreTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == KIntgTest1)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KIntgTest2)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KIntgTest3)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KIntgTest4)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KIntgTest5)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KIntgTest6)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KIntgTest7)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KIntgTest8)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KIntgTest9)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KIntgTest10)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KIntgTest11)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KIntgTest12)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KIntgTest13)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KIntgTest14)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemCreate)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	
	if(aStepName == KSemgetCreateKey)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemgetCreatePrivate)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemgetGetPrivate)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemgetCreateKeyMax)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemgetCreatePvtMax)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemgetGetNonExisting)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemgetCreateInvalidSizeK)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemgetCreateInvalidSizeP)
		{
		testStep = new CTestSemaphore(aStepName);
		}		
	if(aStepName == KSemgetGetMore)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemgetCreateExisting)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemgetGetExisting)
		{
		testStep = new CTestSemaphore(aStepName);
		}		
	if(aStepName == KSemctlInvalidId)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemctlInvalidArgsK)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemctlInvalidArgsP)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemctlStatK)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemctlStatP)
		{
		testStep = new CTestSemaphore(aStepName);
		}		
	if(aStepName == KSemctlSetK)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemctlSetP)
		{
		testStep = new CTestSemaphore(aStepName);
		}		
	if(aStepName == KSemctlGetValK)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == KSemctlGetValP)
		{
		testStep = new CTestSemaphore(aStepName);
		}		
	if(aStepName == Ksemctltest8)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemctltest9)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemctltest10)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemctltest11)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemctltest12)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemctltest13)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemctltest14)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest1)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest2)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest3)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest4)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest5)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest6)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest7)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest8)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest9)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest10)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest11)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest12)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest13)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest14)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	if(aStepName == Ksemoptest15)
		{
		testStep = new CTestSemaphore(aStepName);
		}
	return testStep;
	}

