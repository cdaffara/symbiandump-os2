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
// Name        : tclockserver.cpp
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

#include "tclockserver.h"
#include "tclock.h"


_LIT(KServerName, "tclock");

CClockTestServer* CClockTestServer::NewL()
	{
	CClockTestServer *server = new(ELeave) CClockTestServer();
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
	CClockTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CClockTestServer::NewL());
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

CTestStep* CClockTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTestgetclockid1)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestgetclockid2)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestgetclockid3)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestgetclockid4)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestgetclockid5)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclockresolution1)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclockresolution2)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclockresolution3)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclocknanosleep1)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclocknanosleep2)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclocknanosleep3)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclocknanosleep4)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclocknanosleep5)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclocknanosleep6)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclockgettime1)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclockgettime2)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclockgettime3)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclocksettime1)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclocksettime2)
		{
			testStep = new CTestclock(aStepName);
		}
	if(aStepName == KTestclocksettime3)
		{
			testStep = new CTestclock(aStepName);
		}
	return testStep;
	 }

