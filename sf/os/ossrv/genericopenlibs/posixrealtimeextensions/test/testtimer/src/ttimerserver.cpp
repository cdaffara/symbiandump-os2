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
// Name        : ttimerserver.cpp
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
#include "ttimerserver.h"
#include "ttimer.h"


_LIT(KServerName, "ttimer");

CTimerTestServer* CTimerTestServer::NewL()
	{
	CTimerTestServer *server = new(ELeave) CTimerTestServer();
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
	CTimerTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CTimerTestServer::NewL());
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

CTestStep* CTimerTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTesttimerapi1)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi2)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi3)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi4)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi5)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi6)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi7)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi8)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi9)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi10)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi11)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi12)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi13)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi14)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi15)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi16)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi17)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi18)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi19)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi20)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi21)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi22)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi23)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi24)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi25)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi26)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi27)
		{
			testStep = new CTesttimer(aStepName);
		}
	if(aStepName == KTesttimerapi28)
        {
            testStep = new CTesttimer(aStepName);
        }
	return testStep;
	 }

