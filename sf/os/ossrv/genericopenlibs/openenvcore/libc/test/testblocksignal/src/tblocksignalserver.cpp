// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tblocksignalserver.cpp
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

#include "tblocksignalserver.h"
#include "tblocksignal.h"


_LIT(KServerName, "tblocksignal");

CBlocksignalTestServer* CBlocksignalTestServer::NewL()
	{
	CBlocksignalTestServer *server = new(ELeave) CBlocksignalTestServer();
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
	CBlocksignalTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CBlocksignalTestServer::NewL());
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

CTestStep* CBlocksignalTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTestempty1)
		{
			testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestempty2)
		{
			testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestaddset2)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestaddset3)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestaddset4)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestaddset5)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestaddndelset1)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestaddndelset2)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestaddndelset3)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestaddndelset4)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestaddset1)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigprocmask1)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigprocmask2)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigprocmask3)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigprocmask4)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigprocmask5)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigprocmask6)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigprocmask7)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigprocmask8)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigfillset1)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigfillset2)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigandset1)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigandset2)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigandset3)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigorset1)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigorset2)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigorset3)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestandorset1)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestandorset2)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestandorset3)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsighold1)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsighold2)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigrelse1)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigrelse2)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestholdnrelse1)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestsigpause1)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	if(aStepName == KTestnegsigpause1)
		{
		testStep = new CTestBlocksignal(aStepName);
		}
	return testStep;
	 }

