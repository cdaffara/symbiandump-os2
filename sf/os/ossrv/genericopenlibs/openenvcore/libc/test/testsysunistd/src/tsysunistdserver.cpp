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
#include "tsysunistdserver.h"
#include "tsysunistd.h"


_LIT(KServerName, "tsysunistd");

CSysunistdTestServer* CSysunistdTestServer::NewL()
	{
	CSysunistdTestServer *server = new(ELeave) CSysunistdTestServer();
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
	CSysunistdTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CSysunistdTestServer::NewL());
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

CTestStep* CSysunistdTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps

	if(aStepName == Kftruncate_file)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kftruncate_dir)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kftruncate_num)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kftruncate_siz)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kfchdir_dir)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kfchdir_file)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kfchdir_num)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kfdatasync_file)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kgetpagesize)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kgetpriority)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kgetpriority_other)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Ksetpriority_get)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Ksetpriority_high)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Ksetpriority_other_lower)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Ksetpriority_other_higher)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Ksetpriority_other)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Knice_within_range)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Knice_more_than_highest)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Knice_less_than_lowest)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kpathconf_valid)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kpathconf_invalid)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kuname)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kuname_null)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kftruncate_negTest)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Ktruncate_negTest)
		{
		testStep = new CTestSysunistd(aStepName);
		}
	if(aStepName == Kfcntl_largefileTest)
		{
		testStep = new CTestSysunistd(aStepName);
		}	
	return testStep;
	}

